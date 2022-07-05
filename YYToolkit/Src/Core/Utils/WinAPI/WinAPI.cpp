#include "../../Features/API/Internal.hpp"
#include "WinAPI.hpp"
#include <Psapi.h>

namespace Utils
{
	namespace WinAPI
	{
		// Returns the start address of a thread based off it's handle
		NTSTATUS QueryThreadStartAddress(HANDLE ThreadHandle, uintptr_t& outAddr)
		{
			return NtQueryInformationThread(
				ThreadHandle, 
				(THREADINFOCLASS)9 /*ThreadQuerySetWin32StartAddress*/, 
				&outAddr, 
				sizeof(uintptr_t), 
				nullptr
			);
		}

		// Allocates a buffer containing information about all the processes
		SystemProcessInformation_t* GetProcessInformation()
		{
			SystemProcessInformation_t* Information = nullptr;
			ULONG RequestedSize = 0;

			// Request the size
			NtQuerySystemInformation(
				SystemProcessInformation,
				Information,
				0,
				&RequestedSize
			);

			// We couldn't get a size
			if (RequestedSize == 0)
				return nullptr;

			// Allocate the requested bytes
			Information = reinterpret_cast<SystemProcessInformation_t*>(malloc(RequestedSize));

			// Get the actual information
			NTSTATUS Status = NtQuerySystemInformation(
				SystemProcessInformation,
				Information,
				RequestedSize,
				nullptr
			);
			
			if (NT_SUCCESS(Status))
				return Information;

			free(Information);
			return nullptr;
		}

		SystemProcessInformation_t* GetProcessEntry(SystemProcessInformation_t* ProcessInformation, uint32_t PID)
		{
			while (ProcessInformation->NextEntryOffset)
			{
				if (reinterpret_cast<uint32_t>(ProcessInformation->ProcessId) == PID)
					return ProcessInformation;

				// Advance to the next entry
				ProcessInformation = reinterpret_cast<SystemProcessInformation_t*>(
					reinterpret_cast<PBYTE>(ProcessInformation) + ProcessInformation->NextEntryOffset
				);
			}

			// Check the last entry in the list (probs not it but let's be safe)
			if (reinterpret_cast<uint32_t>(ProcessInformation->ProcessId) == PID)
				return ProcessInformation;

			return nullptr;
		}

		bool IsMainProcessSuspended()
		{
			// Get the game process
			auto ProcessInformation = GetProcessInformation();

			if (!ProcessInformation)
				return false;

			auto GameProcess = GetProcessEntry(ProcessInformation, GetCurrentProcessId());

			if (!GameProcess)
				return false;

			MODULEINFO GameProcessInfo;
			GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(nullptr), &GameProcessInfo, sizeof(MODULEINFO));

			for (size_t i = 0; i < GameProcess->NumberOfThreads; i++)
			{
				// Open the thread
				HANDLE hThread = OpenThread(
					THREAD_ALL_ACCESS, 
					FALSE, 
					reinterpret_cast<DWORD>(GameProcess->Threads[i].ClientId.UniqueThread)
				);

				uintptr_t ThreadStart = 0;
				// If we can't get the thread start address, skip the thread
				if (!NT_SUCCESS(QueryThreadStartAddress(hThread, ThreadStart)))
				{
					CloseHandle(hThread);
					continue;
				}

				if (ThreadStart != reinterpret_cast<uintptr_t>(GameProcessInfo.EntryPoint))
				{
					CloseHandle(hThread);
					continue;
				}

				CloseHandle(hThread);
				return (GameProcess->Threads[i].State == KThreadState::Waiting) &&
					(GameProcess->Threads[i].WaitReason == KWaitReason::Suspended);
			}

			return false;
		}
	}
}

