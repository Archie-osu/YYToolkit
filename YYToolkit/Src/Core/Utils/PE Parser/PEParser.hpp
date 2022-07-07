#ifndef _YYTK_UTILS_PEPARSER_H_
#define _YYTK_UTILS_PEPARSER_H_
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>

namespace Utils
{
	namespace PE
	{
		DWORD RVA_To_Offset(PIMAGE_NT_HEADERS pNTHeader, DWORD dwRVA);

		bool DoesPEExportRoutine(const wchar_t* FilePath, const char* RoutineName);

		bool IsPEx64(const wchar_t* FilePath);
	}
}

#endif // _YYTK_UTILS_PEPARSER_H_
