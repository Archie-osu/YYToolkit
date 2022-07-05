#ifndef _YYTK_CORE_PLUGINMANAGER_H_
#define _YYTK_CORE_PLUGINMANAGER_H_
#include <string>
#include <functional>
namespace PM
{
	struct JSONData
	{
		int Version; // YYTK major version supported by the mod
		bool NeedsPreload; // True if the mod needs Early Launch
		std::string ModID; // The actual ID of the mod - do not change once released!
		std::string Name; // The user-readable name of the mod
		std::string Description; // The description of the mod
	};

	struct ModEntry
	{
		uintptr_t ModuleBase;
		//std::function whatever
		JSONData Data;
	};

	bool ReadManifest(const wchar_t* FilePath, JSONData& outData);
	bool WriteManifest(const wchar_t* FilePath, JSONData& outData);

	bool CreatePluginData(const wchar_t* FolderPath, ModEntry& outEntry);
}

#endif // _YYTK_CORE_PLUGINMANAGER_H_
