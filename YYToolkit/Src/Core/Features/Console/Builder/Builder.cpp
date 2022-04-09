#include "Builder.hpp"
#include <regex>

#define NPOS std::string::npos

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

				if (FunctionCall.at(index) == ',')
					ArgCount++;
			}

			return ArgCount;
		}

		size_t GetNthOccurence(const std::string& where, char what, int which)
		{
			int Count = 0;

			for (size_t n = 0; n < where.length(); n++)
			{
				if (where.at(n) != what)
					continue;

				if (Count == which)
					return n;

				Count++;
			}

			return NPOS;
		}

		Argument_t MakeArgument(const std::string& FunctionCall, uint32_t Index)
		{
			// Return empty if index < 1
			if (Index < 1)
				return { "", false };

			// If we try to access OOB
			if (Index > GetArgumentCount(FunctionCall))
				return { "", false };
			
			// TODO: Somehow get n'th argument, initialize argument like this (or via the constructor)
			Argument_t Argument("<default string>");

			std::string ArgumentContent = "";
			Argument.IsEvaluationRequired = IsFunctionCall(ArgumentContent);
			
			return Argument;
		}

		bool IsFunctionCall(const std::string& RawInput)
		{
			std::regex FunctionCallRegex = std::regex("[a-zA-Z]+\\(.*\\)");
			
			return std::regex_match(RawInput, FunctionCallRegex);
		}

		bool BuildAFD(const std::string& RawInput, FunctionCall_t& out)
		{
			if (!IsFunctionCall(RawInput))
				return false;

			// Find all arguments
			for (int Argument = 1; Argument < GetArgumentCount(RawInput); Argument++)
			{
				out.Arguments.push_back(MakeArgument(RawInput, Argument));
			}

			return true;
		}
	}
}