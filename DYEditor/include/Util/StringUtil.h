#pragma once

#include <string>
#include <cctype>

namespace DYE::DYEditor::StringUtil
{
	/// Add a number suffix (e.g., "(17)") at the end of the string.
	/// If a number suffix is already presented in the string, we increment the value and replace the original number.
	/// For example, "EntityName (0)" will become "EntityName (1)".
	std::string AddOrIncrementNumberSuffix(std::string input)
	{
		if (input.length() < 3)
		{
			input += "(1)";
			return input;
		}

		if (!input.ends_with(')'))
		{
			input += "(1)";
			return input;
		}

		char const secondLastChar = input[input.length() - 2];
		if (!std::isdigit(secondLastChar))
		{
			input += "(1)";
			return input;
		}

		int numSuffixStartIndex = 0;
		for (int i = input.length() - 3; i >= 0; i--)
		{
			char const character = input[i];
			if (std::isdigit(character))
			{
				continue;
			}

			bool reachedStartOfNumSuffix = character == '(';
			if (reachedStartOfNumSuffix)
			{
				numSuffixStartIndex = i;
				break;
			}

			input += "(1)";
			return input;
		}

		int const numberStartIndex = numSuffixStartIndex + 1;
		int const numberOfDigits = input.length() - numberStartIndex - 1;
		std::string numberStr = input.substr(numberStartIndex, numberOfDigits);
		int number = std::stoi(numberStr);
		number++;
		input.replace(numberStartIndex, numberOfDigits, std::to_string(number));
		return input;
	}
}