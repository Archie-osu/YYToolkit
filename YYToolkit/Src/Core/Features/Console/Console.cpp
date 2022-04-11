#include "../../SDK/Structures/Documented/YYRValue/YYRValue.hpp"
#include "../../SDK/Structures/Documented/RefThing/RefThing.hpp"
#include "../../Utils/Logging/Logging.hpp"
#include "Builder/Builder.hpp"
#include "../API/API.hpp"
#include "Console.hpp"

std::string GetInput()
{
	// Run in global scope
	CInstance* pInstance = API::gAPIVars.Globals.g_pGlobalInstance;

	// Prepare result buffer
	YYRValue Buffer;

	if (!API::CallBuiltin(Buffer, "get_string", pInstance, nullptr, { "Please input your expression:", "" }))
	{
		Utils::Logging::Error(
			__FILE__,
			__LINE__,
			"API::CallBuiltin returned false"
		);

		return "";
	}

	return (Buffer.operator std::string());
	
}

void Console::RunCommand(std::string _cmd)
{
	if (_cmd.empty())
	{
		_cmd = GetInput();

		if (_cmd.empty())
			return;
	}

	auto Tokens = Builder::BuildTokenList(_cmd);

	if (Tokens.empty())
		return;

	std::vector<YYRValue> Arguments;
	YYRValue Result;

	TreeNode_t AST = Builder::BuildAST(Tokens);

	// The first possible call is at MaxDepth - 1, since even an empty call
	// would have the () on their own depth
	int MaxDepth = Builder::GetMaxTreeDepth(AST);

	for (int CurrDepth = MaxDepth; CurrDepth >= 0; CurrDepth--)
	{
		for (auto& Node : Builder::GetNodesAtDepth(AST, CurrDepth))
		{
			if (!Node.Token)
				continue;

			if (Node.Token->Type == ETokenKind::TokenKind_String)
				Arguments.push_back(YYRValue(Node.Token->Value));

			else if (Node.Token->Type == ETokenKind::TokenKind_Number)
				Arguments.push_back(YYRValue(std::stod(Node.Token->Value)));

			if (Node.Token->Type == ETokenKind::TokenKind_Identifier)
			{
				if (API::CallBuiltin(Result, Node.Token->Value, nullptr, nullptr, Arguments))
				{
					Arguments.clear();
					Arguments.push_back(Result);
					continue;
				}

				Utils::Logging::Error(__FILE__, __LINE__, "Unknown identifier \"%s\" - not executing this!", Node.Token->Value.c_str());
				return;
			}
		}
	}

	Utils::Logging::NoNewlineMessage(CLR_GOLD, "%s", _cmd.c_str());
	Utils::Logging::NoNewlineMessage(CLR_DEFAULT, " -> ");

	switch (Result.As<RValue>().Kind)
	{
	case VALUE_REAL:
		Utils::Logging::Message(CLR_BLUE, "%.2f", Result.As<RValue>().Real);
		break;
	case VALUE_STRING:
		Utils::Logging::Message(CLR_YELLOW, "\"%s\"", Result.As<RValue>().String->Get());
		break;
	case VALUE_PTR:
		Utils::Logging::Message(CLR_BLUE, "Pointer to 0x%p", Result.As<RValue>().Pointer);
		break;
	case VALUE_OBJECT:
		Utils::Logging::Message(CLR_BLUE, "Object at 0x%p", Result.As<RValue>().Pointer);
		break;
	case VALUE_INT32:
		Utils::Logging::Message(CLR_BLUE, "%d (0x%X)", Result.As<RValue>().I32, Result.As<RValue>().I32);
		break;
	case VALUE_INT64:
		Utils::Logging::Message(CLR_BLUE, "%lld (0x%llX)", Result.As<RValue>().I64, Result.As<RValue>().I64);
		break;
	case VALUE_BOOL:
		Utils::Logging::Message(CLR_TANGERINE, "%s", (Result.As<RValue>().Real > 0.5) ? "true" : "false");
		break;
	case VALUE_UNDEFINED:
		Utils::Logging::Message(CLR_GRAY, "<undefined>");
		break;
	case VALUE_UNSET:
		Utils::Logging::Message(CLR_GRAY, "<unset>");
		break;
	default:
		Utils::Logging::Message(CLR_GRAY, "Unknown return type (0x%X)", Result.As<RValue>().Kind);
	}
}
