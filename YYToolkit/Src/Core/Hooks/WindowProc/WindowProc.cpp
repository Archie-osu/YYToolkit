#include "WindowProc.hpp"
#include "../../Utils/Logging/Logging.hpp"
#include "../../Features/API/API.hpp"
#include "../../Features/PluginManager/PluginManager.hpp"
#include "../../SDK/Plugins/FunctionWrapper/FunctionWrapper.hpp"

namespace Hooks
{
	namespace WindowProc
	{
		LRESULT __stdcall Function(HWND hwnd, unsigned int Msg, WPARAM w, LPARAM l)
		{
			if (Msg == WM_CLOSE)
				exit(0);

			FWWindowProc Event(pfnOriginal, hwnd, Msg, w, l);
			PM::InvokeCallbacks(EventType::kWndProc, Event);

			if (Event.CalledOriginal())
				return Event.Result();

			return CallWindowProc(pfnOriginal, hwnd, Msg, w, l);
		}

		void _SetWindowsHook()
		{
			pfnOriginal = reinterpret_cast<WNDPROC>(SetWindowLongPtr(reinterpret_cast<HWND>(API::gAPIVars.Globals.g_hwWindowHandle), GWLP_WNDPROC, reinterpret_cast<uintptr_t>(Function)));
		}
	}
}