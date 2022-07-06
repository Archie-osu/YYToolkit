#include <iostream>
#include <Windows.h>

// Prepare for trouble...
#include <string>
// ... and make it double!
#include <filesystem> 

#include "Argengine/Argengine.hpp"

int main(int argc, char** argv)
{
	juzzlin::Argengine Engine(argc, argv, false);

	std::string RunnerPath;
	std::string DataPath;
	std::string InjectPath;
	bool EnableEarlyLaunch = false;

	// Register options 
	Engine.addOption({ "-h", "--help" }, [] {
		std::cout << "YYToolkit CLI" << std::endl;
		std::cout << "-h, --help | Display this screen" << std::endl;
		std::cout << "-rf, --runnerfile <full_file_path> | Runner (game executable) file" << std::endl;
		std::cout << "-df, --datafile <full_file_path> | The data.win file to be used" << std::endl;
		std::cout << "-if, --injectfile <full_file_path> | File to inject" << std::endl;
		std::cout << "-el, --earlylaunch | Enable Early Launch" << std::endl << std::endl;

		std::cout << "Warning! Only UTF-8 paths are supported!" << std::endl;
	});

	Engine.addOption({ "-rf", "--runnerfile" }, [&RunnerPath] (std::string Path) {
		std::cout << "Registered runner path: \"" << Path << "\"" << std::endl;
		RunnerPath = Path;
	});

	Engine.addOption({ "-df", "--datafile" }, [&DataPath](std::string Path) {
		std::cout << "Registered custom data.win path: \"" << Path << "\"" << std::endl;
		DataPath = Path;
	});

	Engine.addOption({ "-if", "--injectfile" }, [&InjectPath](std::string Path) {
		std::cout << "Registered file to inject: \"" << Path << "\"" << std::endl;
		InjectPath = Path;
	});

	Engine.addOption({ "-el", "--earlylaunch" }, [&EnableEarlyLaunch]() {
		std::cout << "Enabled early launch support." << std::endl;
		EnableEarlyLaunch = true;
	});

	Engine.parse();

	// Check if we specified at least the runner
	if (RunnerPath.empty())
	{
		std::cout << "Error: No runner file specified - aborting." << std::endl;
		return 0;
	}

	// Check if the file exists
	try
	{
		if (!std::filesystem::exists(RunnerPath))
		{
			std::cout << "Error: Runner file doesn't exist." << std::endl;
			return 0;
		}
	}
	catch (std::filesystem::filesystem_error& ex)
	{
		std::cout << "Error: Runner file cannot be validated." << std::endl;
		std::cout << "Error message: " << ex.what() << std::endl;
		return 0;
	}

	std::string NewProcessArguments = "\"" + RunnerPath + "\"";
	std::string RunnerDirectory;

	try
	{
		RunnerDirectory = std::filesystem::path(RunnerPath).parent_path().string();
	}
	catch (std::exception& ex)
	{
		std::cout << "Error: Failed to get the runner's parent directory." << std::endl;
		std::cout << "Error message: " << ex.what() << std::endl;
		return 0;
	}


	if (!DataPath.empty())
	{
		NewProcessArguments += " -game \"";
		NewProcessArguments += DataPath;
		NewProcessArguments += "\"";
	}

	STARTUPINFOA StartupInfo;
	memset(&StartupInfo, 0, sizeof(STARTUPINFOA));
	StartupInfo.cb = sizeof(STARTUPINFOA);

	PROCESS_INFORMATION ProcessInformation;

	BOOL Success = CreateProcessA(
		nullptr /* fuck you microsoft */,
		NewProcessArguments.data(),
		nullptr,
		nullptr,
		false,
		CREATE_DEFAULT_ERROR_MODE | (EnableEarlyLaunch ? CREATE_SUSPENDED : 0),
		nullptr,
		RunnerDirectory.c_str(),
		&StartupInfo,
		&ProcessInformation
	);

	if (!Success)
	{
		std::cout << "Error: Failed to create the process." << std::endl;
		std::cout << "Error code: 0x" << std::hex << GetLastError() << std::endl;
		return 0;
	}

	if (!EnableEarlyLaunch)
		WaitForInputIdle(ProcessInformation.hProcess, 250);

	Sleep(100); // Sleep for a bit just to be safe

	// Prepare for injection - allocate space in the target process

	void* AllocatedSpace = VirtualAllocEx(
		ProcessInformation.hProcess, 
		nullptr, 
		MAX_PATH, 
		MEM_COMMIT | MEM_RESERVE, 
		PAGE_READWRITE
	);

	// Write our DLL name in
	
	Success = WriteProcessMemory(
		ProcessInformation.hProcess, 
		AllocatedSpace, 
		InjectPath.c_str(), 
		InjectPath.length(),
		nullptr
	);

	if (!Success)
	{
		std::cout << "Error: Failed to inject the DLL." << std::endl;
		std::cout << "Error code: 0x" << std::hex << GetLastError() << std::endl;
		return 0;
	}

	HANDLE hThread = CreateRemoteThread(
		ProcessInformation.hProcess, 
		nullptr, 
		0, 
		reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryA), 
		AllocatedSpace, 
		0, 
		nullptr
	);

	std::cout << "Injection successful." << std::endl;

	CloseHandle(hThread);
	CloseHandle(ProcessInformation.hProcess);
	CloseHandle(ProcessInformation.hThread);
}