#pragma once
#include "../../SDK/FwdDecls/FwdDecls.hpp"
#include "Structures/PmStructures.hpp"

namespace API 
{
	namespace PluginManager 
	{
		inline std::list<PluginAttributes_t> g_PluginStorage;

		YYTKPlugin* LoadPlugin(const char* Path);
		bool UnloadPlugin(YYTKPlugin& pPlugin, bool Notify);

		bool IsPluginCompatible(HMODULE Plugin);

		void RunHooks(YYTKEventBase* pEvent);
		void RunPluginMains();
		void RunPluginPreloads();

		void Initialize();
		void Uninitialize();

		DllExport YYTKStatus PmGetPluginAttributes(YYTKPlugin* pObject, PluginAttributes_t*& outAttributes);
		DllExport YYTKStatus PmCreateCallback(PluginAttributes_t* pObjectAttributes, CallbackAttributes_t*& outAttributes, FNEventHandler pfnCallback, EventType Flags, void* OptionalArgument);
		DllExport YYTKStatus PmRemoveCallback(CallbackAttributes_t* CallbackAttributes);
	}
}