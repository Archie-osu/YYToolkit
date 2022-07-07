#include "../../Utils/PE Parser/PEParser.hpp"
#include "../../Utils/Logging/Logging.hpp"
#undef min // Fuck you Microsoft
#undef max // Fuck you Microsoft x2
#include "../../SDK/FwdDecls/FwdDecls.hpp"
#include "../../Utils/JSON/json.hpp"
#include "PluginManager.hpp"
#include <filesystem>
#include <fstream>


namespace PM
{
	bool ReadManifest(const std::wstring& FilePath, JSONData& Data)
	{
		std::ifstream ManifestFile(FilePath);

		// Error opening file?
		if (ManifestFile.bad())
			return false;

		// Read the manifest
		nlohmann::json Manifest;
		ManifestFile >> Manifest;

		try
		{
			Data.ModID = Manifest["modid"];
			Data.Name = Manifest["name"];
			Data.ApiVersion = Manifest["apiversion"];
			Data.Description = Manifest["description"];
			Data.NeedsPreload = Manifest["needspreload"];
			Data.Exports = Manifest["exports"];
		}
		catch (nlohmann::json::exception& e)
		{
			Utils::Logging::Error(__FILE__, __LINE__, "Error parsing plugin manifest: %s", e.what());
			return false;
		}

		return true;
	}

	bool WriteManifest(const std::wstring& FilePath, JSONData& Data)
	{
		std::ofstream ManifestFile("file.json");

		// Error opening file?
		if (ManifestFile.bad())
			return false;

		// Read the manifest
		nlohmann::json Manifest;

		try
		{
			Manifest["modid"] = Data.ModID;
			Manifest["name"] = Data.Name;
			Manifest["description"] = Data.Description;
			Manifest["apiversion"] = Data.ApiVersion;
			Manifest["needspreload"] = Data.NeedsPreload;
			Manifest["exports"] = Data.Exports;
		}
		catch (nlohmann::json::exception& e)
		{
			Utils::Logging::Error(__FILE__, __LINE__, "Error writing json object: %s", e.what());
			return false;
		}

		ManifestFile << std::setw(4) << Manifest;

		return true;
	}

	bool IsPEArchitectureCompatible(const std::wstring& FilePath)
	{
		// Thank you nik
		return sizeof(int*) == (Utils::PE::IsPEx64(FilePath.c_str()) ? 8 : 4);
	}

	bool LoadPlugin(const std::wstring& FilePath, const JSONData& JsonData, PluginDataInternal& InternalData)
	{
		HMODULE hModule = LoadLibraryW(FilePath.c_str());

		if (!hModule || hModule == INVALID_HANDLE_VALUE)
		{
			Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - error code 0x%X",
				JsonData.Name.c_str(),
				GetLastError()
			);
			return false;
		}

		InternalData.Data = JsonData;
		InternalData.ModuleBase = reinterpret_cast<uintptr_t>(hModule);

		using YYTKFunc = void(*)(PluginDescriptor& Descriptor);

		// PluginEntry in older terms
		YYTKFunc ModEntry = reinterpret_cast<YYTKFunc>(GetProcAddress(hModule, "ModEntry"));
		if (ModEntry == nullptr)
		{
			Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - entry routine not found",
				JsonData.Name.c_str()
			);
			
			FreeLibrary(hModule);
			return false;
		}
		InternalData.Descriptor.m_ModEntry = ModEntry;

		// We're done if we don't need to worry about Early Launch stuff
		if (!JsonData.NeedsPreload)
			return true;

		// PluginPreload in older terms
		YYTKFunc ModPreload = reinterpret_cast<YYTKFunc>(GetProcAddress(hModule, "ModPreload"));
		if (ModPreload == nullptr)
		{
			Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - preload routine not found",
				JsonData.Name.c_str()
			);

			FreeLibrary(hModule);
			return false;
		}
		InternalData.Descriptor.m_ModPreload = ModPreload;
		
		return true;
	}

	bool UnloadPlugin(const std::string& ModID)
	{
		for (auto& Plugin : g_Plugins)
		{
			if (ModID != Plugin.Data.ModID)
				continue;

			if (Plugin.Descriptor.m_ModUnload)
				Plugin.Descriptor.m_ModUnload(Plugin.Descriptor);

			return FreeLibrary(reinterpret_cast<HMODULE>(Plugin.ModuleBase));
		}

		return false;
	}

	void InvokeModEntryRoutines()
	{
		for (auto& Plugin : g_Plugins)
				Plugin.Descriptor.m_ModEntry(Plugin.Descriptor);
	}

	void InvokeModPreloadRoutines()
	{
		for (auto& Plugin : g_Plugins)
			if (Plugin.Data.NeedsPreload)
				Plugin.Descriptor.m_ModPreload(Plugin.Descriptor);
	}

	void Initialize(const std::wstring& FolderName)
	{
		namespace fs = std::filesystem;

		if (!fs::exists(FolderName))
			return;

		try
		{
			// Loop every directory
			for (auto& Entry : fs::directory_iterator(FolderName))
			{
				if (!Entry.is_directory())
					continue;

				// There's a potential manifest file - it's always manifest.json
				fs::path PotentialManifestFile = Entry.path().wstring().append(L"/manifest.json");

				// Check for it's existence
				if (!fs::exists(PotentialManifestFile))
					continue;

				JSONData Data;
				bool ReadSuccess = ReadManifest(PotentialManifestFile.wstring(), Data);

				// Data.Version will not be checked if ReadSuccess is false
				// This is because operator&& guarantees LTR evaluation
				if (ReadSuccess)
				{
					// There should be a module.dll containing the plugin code
					fs::path PotentialPluginFile = Entry.path().wstring().append(L"/module.dll");

					if (Data.ApiVersion != YYSDK_VERSION)
					{
						Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - needs API version %d, but you're running %d",
							Data.Name.c_str(),
							Data.ApiVersion,
							YYSDK_VERSION
						);
						continue;

					}

					// Check for it's existence
					if (!fs::exists(PotentialPluginFile))
					{
						Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - module file not found",
							Data.Name.c_str()
						);
						continue;
					}

					if (!IsPEArchitectureCompatible(PotentialPluginFile))
					{
						Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - module architecture is incompatible",
							Data.Name.c_str()
						);
						continue;
					}

					if (!Utils::PE::DoesPEExportRoutine(PotentialPluginFile.wstring().c_str(), "ModEntry"))
					{
						Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - incorrect format",
							Data.Name.c_str()
						);
						continue;
					}

					if (Data.NeedsPreload && (!Utils::PE::DoesPEExportRoutine(PotentialPluginFile.wstring().c_str(), "ModPreload")))
					{
						Utils::Logging::Error(__FILE__, __LINE__, "Plugin \"%s\" not loaded - incorrect format",
							Data.Name.c_str()
						);
						continue;
					}

					PluginDataInternal PluginData;
					if (LoadPlugin(PotentialPluginFile, Data, PluginData))
					{
						g_Plugins.emplace_back(PluginData);
						Utils::Logging::Message(CLR_LIGHTBLUE, "Loaded plugin \"%s\" - mapped to 0x%X", 
							PluginData.Data.Name.c_str(),
							PluginData.ModuleBase
						);
					}
				}
			}
		}
		catch (std::exception& ex)
		{
			Utils::Logging::Error(__FILE__, __LINE__, "Unexpected error while loading plugins - %s", ex.what());
		}
	}
	void Uninitialize()
	{
		for (auto it = g_Plugins.begin(); it != g_Plugins.end();)
		{
			UnloadPlugin(it->Data.ModID);
			it = g_Plugins.erase(it);
		}

		g_Plugins.clear(); // Just in case.
	}
}