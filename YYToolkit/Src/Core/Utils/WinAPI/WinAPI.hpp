#pragma once
#include <Windows.h>
#include <winternl.h>
#include <cstdint>

namespace Utils
{
	namespace WinAPI
	{
		enum KWaitReason
		{
			Executive = 0,
			FreePage = 1,
			PageIn = 2,
			PoolAllocation = 3,
			DelayExecution = 4,
			Suspended = 5,
			UserRequest = 6,
			WrExecutive = 7,
			WrFreePage = 8,
			WrPageIn = 9,
			WrPoolAllocation = 10,
			WrDelayExecution = 11,
			WrSuspended = 12,
			WrUserRequest = 13,
			WrEventPair = 14,
			WrQueue = 15,
			WrLpcReceive = 16,
			WrLpcReply = 17,
			WrVirtualMemory = 18,
			WrPageOut = 19,
			WrRendezvous = 20,
			Spare2 = 21,
			Spare3 = 22,
			Spare4 = 23,
			Spare5 = 24,
			WrCalloutStack = 25,
			WrKernel = 26,
			WrResource = 27,
			WrPushLock = 28,
			WrMutex = 29,
			WrQuantumEnd = 30,
			WrDispatchInt = 31,
			WrPreempted = 32,
			WrYieldExecution = 33,
			WrFastMutex = 34,
			WrGuardedMutex = 35,
			WrRundown = 36,
			MaximumWaitReason = 37
		};

		enum KThreadState
		{
			Initialized,
			Ready,
			Running,
			Standby,
			Terminated,
			Waiting,
			Transition,
			DeferredReady,
			GateWaitObsolete,
			WaitingForProcessInSwap,
			MaximumThreadState
		};

		struct ClientID
		{
			HANDLE UniqueProcess;
			HANDLE UniqueThread;
		};

		struct SystemThread_t {
			LARGE_INTEGER           KernelTime;
			LARGE_INTEGER           UserTime;
			LARGE_INTEGER           CreateTime;
			ULONG                   WaitTime;
			PVOID                   StartAddress;
			ClientID				ClientId;
			LONG					Priority;
			LONG                    BasePriority;
			ULONG                   ContextSwitchCount;
			KThreadState            State;
			KWaitReason             WaitReason;

		};

		struct VMCounters_t
		{
			SIZE_T PeakVirtualSize;
			SIZE_T VirtualSize;
			ULONG PageFaultCount;
			SIZE_T PeakWorkingSetSize;
			SIZE_T WorkingSetSize;
			SIZE_T QuotaPeakPagedPoolUsage;
			SIZE_T QuotaPagedPoolUsage;
			SIZE_T QuotaPeakNonPagedPoolUsage;
			SIZE_T QuotaNonPagedPoolUsage;
			SIZE_T PagefileUsage;
			SIZE_T PeakPagefileUsage;
		};

		struct SystemProcessInformation_t {
			ULONG                   NextEntryOffset;
			ULONG                   NumberOfThreads;
			LARGE_INTEGER           Reserved[3];
			LARGE_INTEGER           CreateTime;
			LARGE_INTEGER           UserTime;
			LARGE_INTEGER           KernelTime;
			UNICODE_STRING          ImageName;
			KPRIORITY               BasePriority;
			HANDLE                  ProcessId;
			HANDLE                  InheritedFromProcessId;
			ULONG                   HandleCount;
			ULONG                   Reserved2[2];
			ULONG                   PrivatePageCount;
			VMCounters_t            VirtualMemoryCounters;
			IO_COUNTERS             IoCounters;
			SystemThread_t          Threads[0];
		};

		NTSTATUS QueryThreadStartAddress(HANDLE ThreadHandle, uintptr_t& outAddr);

		SystemProcessInformation_t* GetProcessInformation();

		SystemProcessInformation_t* GetProcessEntry(SystemProcessInformation_t* ProcessInformation, uint32_t PID);

		bool IsMainProcessSuspended();
	}
}
