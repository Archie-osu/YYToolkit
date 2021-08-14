#pragma once
#ifdef __cplusplus
#define DllExport extern "C" __declspec(dllexport)
#else //!__cplusplus
#define DllExport __declspec(dllexport)
#endif //__cplusplus

// Forward declarations
#include "FwdDecls/FwdDecls.hpp"

// Enums
#include "Enums/Enums.hpp"

// Documented
#include "Structures/Documented/CCode/CCode.hpp"
#include "Structures/Documented/CDynamicArray/CDynamicArray.hpp"
#include "Structures/Documented/FunctionInfo/FunctionInfo.hpp"
#include "Structures/Documented/CHashMap/CHashMap.hpp"
#include "Structures/Documented/Math/Math.hpp"
#include "Structures/Documented/RefThing/RefThing.hpp"
#include "Structures/Documented/RToken/RToken.hpp"
#include "Structures/Documented/VMBuffer/VMBuffer.hpp"
#include "Structures/Documented/YYRValue/YYRValue.hpp"

// Undocumented - these may not work on all GMS versions (aka. may require plugin-specific additions / removals)

#include "Structures/Undocumented/YYGMLFuncs/YYGMLFuncs.hpp"
#include "Structures/Undocumented/YYObjectBase/YYObjectBase.hpp"
#include "Structures/Undocumented/YYVAR/YYVAR.hpp"

// Plugins.. duh
#include "Plugins/Plugins.hpp"