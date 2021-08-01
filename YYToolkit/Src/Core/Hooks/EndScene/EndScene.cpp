#include "EndScene.hpp"
#include "../../Features/API/API.hpp"
#include "../../Utils/Error.hpp"

namespace Hooks::EndScene
{
	HRESULT __stdcall Function(LPDIRECT3DDEVICE9 _this)
	{
		auto Result = pfnOriginal(_this);

		return Result;
	}

	void* GetTargetAddress()
	{
		void* ppTable[119];

		memcpy(ppTable, *(void***)(gAPIVars.Window_Device), sizeof(ppTable));

		if (!ppTable[42])
			Utils::Error::Error(1, "Failed to get the EndScene function pointer.");

		return ppTable[42];
	}
}	