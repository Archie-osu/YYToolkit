#include "Builder.hpp"
#include "../../../Utils/Logging/Logging.hpp"
#include <regex>

namespace Console
{
	namespace Builder
	{
		uint32_t GetArgumentCount(const std::string& FunctionCall)
		{
			// Assume it's already a function call

			size_t CallStart = FunctionCall.find_first_of('(');
			size_t CallEnd = FunctionCall.find_last_of(')');

			// Signifies how deep we are in function calls
			int CurrentDepth = 0;

			// We know there's one arg if there's something between the parentheses
			int ArgCount = static_cast<int>(CallEnd - CallStart) > 1;

			// Go from the start of the first arg to 2nd
			for (size_t index = CallStart + 1; index < CallEnd; index++)
			{
				if (FunctionCall.at(index) == '(')
					CurrentDepth++;
				else if (FunctionCall.at(index) == ')')
					CurrentDepth--;

				// Only count top-level args
				if (CurrentDepth == 0)
				{
					if (FunctionCall.at(index) == ',')
						ArgCount++;
				}
			}

			return ArgCount;
		}

		std::string RemoveWS(const std::string& FunctionCall)
		{
			bool IsInsideString = false;

			std::string NewString;

			for (int n = 0; n < FunctionCall.length(); n++)
			{
				if (FunctionCall.at(n) == '"')
					IsInsideString = !IsInsideString;

				if (FunctionCall.at(n) != ' ' || IsInsideString)
					NewString.push_back(FunctionCall.at(n));
			}

			return NewString;
		}

		std::string RemoveWSFull(const std::string& RawInput)
		{
			std::string NewString = RawInput;

			// Remove spaces
			NewString.erase(std::remove(NewString.begin(), NewString.end(), ' '), NewString.end());
			NewString.erase(std::remove(NewString.begin(), NewString.end(), '\t'), NewString.end());

			return NewString;
		}

		bool IsFunctionCall(const std::string& RawInput)
		{
			std::regex FunctionCallRegex = std::regex("[a-zA-Z_@]+\\(.*\\)");

			return std::regex_match(RawInput, FunctionCallRegex);
		}

		bool IsValidTokenCharacter(char c)
		{
			// I know I can inline this, but I choose readability and maintainability
			// ie. I don't want a giant line of return (x || y || z || w)

			// TokenKind_Opening
			if (c == '(')
				return true;

			// TokenKind_Closing
			if (c == ')')
				return true;

			// TokenKind_String
			if (c == '"')
				return true;

			// TokenKind_Number
			if (isdigit(c))
				return true;

			// TokenKind_EndExpression
			if (c == ';')
				return true;

			return false;
		}

		std::string ResolveShorthands(const std::string& Input)
		{
			// Syntax: global.whatever = value
			// Processed: variable_global_set("whatever", value)
			std::regex regexAssignment("global\\.([a-zA-Z_]+) = (.*)");
			std::regex regexPeek("global\\.([a-zA-Z_]+)");

			std::string Return = Input;

			if (std::regex_match(Return, regexAssignment))
			{
				Return = std::regex_replace(Return, regexAssignment, "variable_global_set(\"$1\", $2)");
			}

			// Syntax: global.whatever
			// Processed: variable_global_get("whatever")
			if (std::regex_match(Return, regexPeek))
			{
				Return = std::regex_replace(Return, regexPeek, "variable_global_get(\"$1\")");
			}

			return Return;
		}

		std::vector<Token_t> BuildTokenList(std::string RawInput)
		{
			if (!IsFunctionCall(RemoveWSFull(RawInput)))
				return {};

			std::string FnCall = RemoveWS(ResolveShorthands(RawInput));

			std::vector<Token_t> Tokens;

			// Loop over every character in the string
			{
				int _CurrDepth = 0;
#define CurrIdx(str) str.at(CurrentIndex)
				std::string UnknownBuffer = ""; // Buffer for unknown parts of the call
				for (int CurrentIndex = 0; CurrentIndex < FnCall.length(); CurrentIndex++)
				{
					// Erase the unknown buffer, the identifier ends here.
					if (IsValidTokenCharacter(CurrIdx(FnCall)) && !UnknownBuffer.empty())
					{
						Tokens.push_back(Token_t(TokenKind_Identifier, UnknownBuffer, CurrentIndex - UnknownBuffer.length(), _CurrDepth));
						UnknownBuffer.clear();
					}

					// Depth Increase
					if (CurrIdx(FnCall) == '(')
					{
						_CurrDepth++;
						Tokens.push_back(Token_t(TokenKind_Opening, "(", CurrentIndex, _CurrDepth));
					}

					// Depth decrease
					else if (CurrIdx(FnCall) == ')')
					{
						Tokens.push_back(Token_t(TokenKind_Closing, ")", CurrentIndex, _CurrDepth));
						_CurrDepth--;
					}

					// String opening
					else if (CurrIdx(FnCall) == '"')
					{
						Token_t Token(TokenKind_String, "", CurrentIndex, _CurrDepth);

						// Until we either reach the end of the string
						// We have to increment CurrentIndex by 1 to not kill the loop immediately
						CurrentIndex++;

						for (; CurrentIndex < FnCall.length() && CurrIdx(FnCall) != '"'; CurrentIndex++)
							Token.Value.push_back(CurrIdx(FnCall));

						Tokens.push_back(Token);
					}

					// Allow numbers starting with - (minus)
					else if (isdigit(CurrIdx(FnCall)) || CurrIdx(FnCall) == '-')
					{
						Token_t Token(TokenKind_Number, "", CurrentIndex, _CurrDepth);

						// We have to account for both digits and decimal points
						while (isdigit(CurrIdx(FnCall)) || CurrIdx(FnCall) == '.' || CurrIdx(FnCall) == '-')
						{
							// Buffer is guaranteed to be empty
							Token.Value.push_back(CurrIdx(FnCall));
							CurrentIndex++;
						}

						Tokens.push_back(Token);

						CurrentIndex--; // I don't know why but this fixes it - maybe a do while loop is the solution?
					}

					else if (CurrIdx(FnCall) == ',')
					{
						Tokens.push_back(Token_t(TokenKind_Separator, ",", CurrentIndex, _CurrDepth));
					}
					
					else if (CurrIdx(FnCall) == ';')
					{
						Tokens.push_back(Token_t(TokenKind_EndExpression, ";", CurrentIndex, _CurrDepth));
						break;
					}
						
					else
					{
						UnknownBuffer.push_back(CurrIdx(FnCall));
					}
				}
#undef CurrIdx
			}

			return Tokens;
		}

		TreeNode_t BuildAST(std::vector<Token_t>& Tokens)
		{
			TreeNode_t Head;

			for (int n = 0; n < Tokens.size(); n++)
			{
				TreeNode_t NewNode;
				NewNode.Token = std::make_optional(Tokens[n]);

				int CurrentDepth = 0;
				int TokenDepth = Tokens[n].Depth;

				TreeNode_t* currNode = &Head;
				while (CurrentDepth != TokenDepth)
				{
					// Vector is empty
					if (currNode->Subnodes.empty())
					{
						currNode->Subnodes.push_back(NewNode);
						break;
					}

					currNode = &currNode->Subnodes.back();
					CurrentDepth++;
				}

				currNode->Subnodes.push_back(NewNode);
			}

			return Head;
		}

		std::vector<TreeNode_t> GetNodesAtDepth(TreeNode_t& Head, int Depth)
		{
			std::vector<TreeNode_t> ResultNodes;

			if (Head.Subnodes.empty())
				return {};

			for (auto& subnode : Head.Subnodes)
			{
				if (!subnode.Token)
					continue;

				if (subnode.Token->Depth == Depth)
					ResultNodes.push_back(subnode);

				std::vector<TreeNode_t> RecursiveVector = GetNodesAtDepth(subnode, Depth);
				for (auto& recurseSubnode : RecursiveVector)
					ResultNodes.push_back(recurseSubnode);
			}

			return ResultNodes;
		}

		/*
		i_suck_at_aiming — Today at 15:11
		do you enjoy trees

		nik — Today at 15:11
		they're neat

		i_suck_at_aiming — Today at 15:12
		do you enjoy searching for the maximum depth of a tree

		(after 20 minutes he realizes)

		nik — Today at 15:32
		jfc i thought you meant like, real life trees
		you said you uninstalled visual studio recently so i thought you're doing forest hiking or something
		*/

		int GetMaxTreeDepth(TreeNode_t& Head)
		{
			for (int n = 0; n < 129; n++)
			{
				if (GetNodesAtDepth(Head, n).empty())
					return n - 1;
			}

			return 0;
		}

		TreeNode_t* FindNodeByStringIndex(TreeNode_t& Head, int StringIndex)
		{
			if (Head.Token)
				if (Head.Token->IndexInString == StringIndex)
					return &Head;

			for (auto& Subnode : Head.Subnodes)
				if (TreeNode_t* Found = FindNodeByStringIndex(Subnode, StringIndex))
					return Found;

			return nullptr;
		}

		std::vector<TreeNode_t> GetFunctionCallArguments(TreeNode_t& BaseNode, bool RemoveUnused)
		{
			std::vector<TreeNode_t> Args;
			for (auto& Subnode : BaseNode.Subnodes)
			{
				if (!Subnode.Token)
					continue;


				if (Subnode.Token->Type == ETokenKind::TokenKind_Identifier)
				{
					std::vector<TreeNode_t> NestedArguments = GetFunctionCallArguments(Subnode);
					for (auto& NestedArgument : NestedArguments)
					{
						if (RemoveUnused)
						{
							if (Subnode.Token->Type == ETokenKind::TokenKind_Opening)
								continue;

							else if (Subnode.Token->Type == ETokenKind::TokenKind_Closing)
								continue;

							else if (Subnode.Token->Type == ETokenKind::TokenKind_Separator)
								continue;
						}

						Args.push_back(NestedArgument);
					}
				}
				else
				{
					if (RemoveUnused)
					{
						if (Subnode.Token->Type == ETokenKind::TokenKind_Opening)
							continue;

						else if (Subnode.Token->Type == ETokenKind::TokenKind_Closing)
							continue;

						else if (Subnode.Token->Type == ETokenKind::TokenKind_Separator)
							continue;
					}

					Args.push_back(Subnode);
				}
			}

			return Args;
		}
	}
}