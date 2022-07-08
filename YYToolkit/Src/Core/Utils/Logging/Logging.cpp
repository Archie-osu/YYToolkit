#include "../../Features/API/API.hpp"
#include "../../SDK/Structures/Documented/YYRValue/YYRValue.hpp"
#include "../../SDK/FwdDecls/FwdDecls.hpp"
#include "Logging.hpp"
#include <Windows.h>

namespace Utils::Logging
{
	void SetPrintColor(Color color)
	{

	}

	void Error(const char* File, const int& Line, const char* fmt, ...)
	{

	}

	void Critical(const char* File, const int& Line, const char* fmt, ...)
	{

	}

	void Message(Color C, const char* fmt, ...)
	{

	}

	void NoNewlineMessage(Color C, const char* fmt, ...)
	{

	}

	std::string YYTKStatus_ToString(YYTKStatus Status)
	{
		switch (Status)
		{
		case YYTK_OK:
			return "YYTK_OK";
		case YYTK_FAIL:
			return "YYTK_FAIL";
		case YYTK_INVALIDARG:
			return "YYTK_INVALIDARG";
		case YYTK_INVALIDRESULT:
			return "YYTK_INVALIDRESULT";
		case YYTK_NOMATCH:
			return "YYTK_NOMATCH";
		case YYTK_UNAVAILABLE:
			return "YYTK_UNAVAILABLE";
		case YYTK_NOT_FOUND:
			return "YYTK_NOT_FOUND";
		default:
			return std::to_string(Status);
		}
	}
}