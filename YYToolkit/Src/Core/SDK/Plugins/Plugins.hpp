#ifndef _YYTK_SDK_PLUGINS_H_
#define _YYTK_SDK_PLUGINS_H_
#include "../Enums/Enums.hpp"
#include <functional>

struct YYTKPlugin;

using FNPluginEntry = YYTKStatus(*)(YYTKPlugin* pPluginObject);
using FNPluginUnload = YYTKStatus(*)();
using FNPluginPreloadEntry = YYTKStatus(*)(YYTKPlugin* pPluginObject);

#pragma pack(push, 1)

struct YYTKPlugin
{
	std::function<void(YYTKPlugin&)> m_ModPreload;
	std::function<void(YYTKPlugin&)> m_ModEntry;
	std::function<void(YYTKPlugin&)> m_ModUnload;
};

#pragma pack(pop)
#endif // _YYTK_SDK_PLUGINS_H_