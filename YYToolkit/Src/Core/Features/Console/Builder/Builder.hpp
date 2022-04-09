#pragma once
#include <string>
#include <vector>
namespace Console
{
	// what do i call this


	struct Argument_t
	{
		std::string Content;
		bool IsEvaluationRequired;

		Argument_t(const std::string& Content)
		{
			this->Content = Content;
			IsEvaluationRequired = false;
		}

		Argument_t(const std::string& Content, bool EvaluationRequired)
		{
			this->Content = Content;
			IsEvaluationRequired = EvaluationRequired;
		}
	};

	struct FunctionCall_t
	{
		// Elements of the function
		// function_test(123, "abc", evalMe()) should become:
			// "function_test" -> false
			// "123" -> false
			// "abc" -> false
			// "evalMe()" -> true
		std::vector<Argument_t> Arguments;
		std::string FunctionName;
	};

	namespace Builder
	{
		// Gets the argument count for a specific function - 0 up to UINT_MAX
		uint32_t GetArgumentCount(const std::string& FunctionCall);

		Argument_t MakeArgument(const std::string& FunctionCall, uint32_t Index);

		size_t GetNthOccurence(const std::string& where, char what, int which);

		// Check if a string is actually a GML-style call
		bool IsFunctionCall(const std::string& RawInput);

		// Builds an AFD structure based off a GML call
		bool BuildAFD(const std::string& RawInput, FunctionCall_t& out);
	}
}