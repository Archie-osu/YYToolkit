#pragma once
#include <string>
#include <vector>
#include <list>
namespace Console
{
	enum ETokenKind
	{
		TokenKind_Identifier, // Whatever is not listed below
		TokenKind_Opening, // Opening brace '('
		TokenKind_Closing, // Closing brace ')'
		TokenKind_String, // "abc"
		TokenKind_Number, // 123
		TokenKind_Separator, // ,
		TokenKind_EndExpression // Semicolons
	};

	struct Token_t
	{
		ETokenKind Type;

		std::string StringRepresentation;
		
		size_t IndexInString;
		int Depth;

		Token_t(ETokenKind Type, const std::string& Value, size_t IndexInString, int Depth)
		{
			this->Type = Type;
			this->StringRepresentation = Value;
			this->IndexInString = IndexInString;
			this->Depth = Depth;
		}
	};

	namespace Builder
	{
		// Gets the argument count for a specific function - 0 up to UINT_MAX
		uint32_t GetArgumentCount(const std::string& FunctionCall);

		// Remove whitespace, doesn't remove it from strings
		std::string RemoveWS(const std::string& FunctionCall);

		// Check if a string is actually a GML-style call
		bool IsFunctionCall(const std::string& RawInput);
		
		// Checks if a character is a token character
		bool IsValidTokenCharacter(char c);

		// Resolves stuff like "global.var = x" to "variable_global_set(var, x)"
		std::string ResolveShorthands(const std::string& Input);

		// Build a token list
		std::vector<Token_t> BuildTokenList(const std::string& RawInput);
	}
}