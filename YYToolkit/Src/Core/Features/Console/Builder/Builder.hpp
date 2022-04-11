#pragma once
#include <string>
#include <vector>
#include <optional>
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
		std::string Value;
		size_t IndexInString;
		int Depth;

		Token_t(ETokenKind Type, const std::string& Value, size_t IndexInString, int Depth)
		{
			this->Type = Type;
			this->Value = Value;
			this->IndexInString = IndexInString;
			this->Depth = Depth;
		}
	};

	struct TreeNode_t
	{
		std::vector<TreeNode_t> Subnodes;
		std::optional<Token_t> Token;
	};

	namespace Builder
	{
		// Gets the argument count for a specific function - 0 up to UINT_MAX
		uint32_t GetArgumentCount(const std::string& FunctionCall);

		// Remove whitespace, doesn't remove it from strings
		std::string RemoveWS(const std::string& FunctionCall);

		// Remove whitespace, don't ignore anything
		std::string RemoveWSFull(const std::string& RawInput);

		// Check if a string is actually a GML-style call
		bool IsFunctionCall(const std::string& RawInput);
		
		// Checks if a character is a token character
		bool IsValidTokenCharacter(char c);

		// Resolves stuff like "global.var = x" to "variable_global_set(var, x)"
		std::string ResolveShorthands(const std::string& Input);

		// Build a token list
		std::vector<Token_t> BuildTokenList(std::string RawInput);

		TreeNode_t BuildAST(std::vector<Token_t>& Tokens);

		std::vector<TreeNode_t> GetNodesAtDepth(TreeNode_t& Head, int Depth);

		int GetMaxTreeDepth(TreeNode_t& Head);

		TreeNode_t& FindNodeByStringIndex(TreeNode_t& Head, int StringIndex);

		std::vector<TreeNode_t> GetFunctionCallArguments(TreeNode_t& BaseNode, bool RemoveUnused = false);
	}
}