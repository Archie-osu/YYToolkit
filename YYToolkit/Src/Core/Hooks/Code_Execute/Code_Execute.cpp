#include "Code_Execute.hpp"
#include "../../Features/API/API.hpp"
#include "../../Features/PluginManager/PluginManager.hpp"
#include "../../Utils/Logging/Logging.hpp"
#include "../../SDK/Plugins/FunctionWrapper/FunctionWrapper.hpp"

namespace Hooks
{
	namespace Code_Execute
	{
		bool Function(CInstance* pSelf, CInstance* pOther, CCode* Code, RValue* Res, int Flags)
		{
			FWExecuteIt Event(pfnOriginal, pSelf, pOther, Code, Res, Flags);
			//API::PluginManager::RunHooks(&Event);

			if (Event.CalledOriginal())
				return Event.Result();

			return pfnOriginal(pSelf, pOther, Code, Res, Flags);
		}

		void* GetTargetAddress()
		{
			return reinterpret_cast<void*>(API::gAPIVars.Functions.Code_Execute);
		}
	}
}