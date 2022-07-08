#ifndef _YYTK_CORE_PLUGINMANAGER_H_
#define _YYTK_CORE_PLUGINMANAGER_H_
#include "../../SDK/Plugins/FunctionWrapper/FunctionWrapper.hpp"
#include "../../SDK/Plugins/Plugins.hpp"
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <list>
namespace PM
{
	struct JSONData
	{
		int ApiVersion; // YYTK API version supported by the mod
		bool NeedsPreload; // True if the mod needs Early Launch
		std::string ModID; // The actual ID of the mod - do not change once released!
		std::string Name; // The user-readable name of the mod
		std::string Description; // The description of the mod
		std::vector<std::string> Exports; // Names of functions exported by the module
	};

	struct PluginDataInternal
	{
		uintptr_t ModuleBase;
		YYTKPlugin Descriptor;
		JSONData Data;
		std::multimap<EventType, void*> Callbacks;
	};

	inline std::list<PluginDataInternal> g_Plugins;

	bool ReadManifest(const std::wstring& FilePath, JSONData& outData);
	bool WriteManifest(const std::wstring& FilePath, JSONData& outData);

	bool IsPEArchitectureCompatible(const std::wstring& FilePath);

	bool LoadPlugin(const std::wstring& FilePath, const JSONData& JsonData, PluginDataInternal& InternalData);
	bool UnloadPlugin(const std::string& ModID);

	void InvokeModEntryRoutines();
	void InvokeModPreloadRoutines();

	template <typename Ret, typename ...Args>
	void InvokeCallbacks(EventType Type, FunctionWrapper<Ret(Args...)>& Scope)
	{
		for (auto& Plugin : g_Plugins)
		{
			for (auto& [Key, Value] : Plugin.Callbacks)
			{
				if (Key == Type)
					reinterpret_cast<void(*)(FunctionWrapper<Ret(Args...)>)>(Value)(Scope);
			}
		}
	}

	// TODO: PmCreateCallback
	// TODO: PmRemoveCallback

	void Initialize(const std::wstring& FolderName);
	void Uninitialize();
}

#endif // _YYTK_CORE_PLUGINMANAGER_H_
