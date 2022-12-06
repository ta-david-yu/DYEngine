#include "Graphics/Processor/UniformPropertyProcessor.h"

#include "Base.h"
#include "Graphics/UniformType.h"

#include <regex>

namespace DYE::ShaderProcessor
{
	void UniformPropertyProcessor::OnBegin(DYE::ShaderProgram &shaderProgram)
	{
		UniformType type;
		//DYE_ASSERT(HasValidPropertyTypeKeywordInLine("#property sampler1D Matrix4x4", type));
		//DYE_LOG(UniformTypeToPropertyTypeQualifier(UniformType::Boolean).c_str());
	}

	void UniformPropertyProcessor::OnPreShaderTypeParse(std::string &programSource)
	{
		m_ShaderProgramSourceCache = programSource;
	}

	void UniformPropertyProcessor::OnEnd(DYE::ShaderProgram &shaderProgram)
	{
		// Acquire uniform information from shader program.
		auto uniformInfos = shaderProgram.GetAllUniformInfo();

		// Parse property information from cached shader program source.
		std::stringstream stream(m_ShaderProgramSourceCache);

		std::string line;
		std::string uniformDeclarationLine;
		while (std::getline(stream, line))
		{
			auto firstToken = getFirstTokenFromLine(line);
			if (firstToken != UniformPropertyDirective)
			{
				// We aren't interested in normal lines. Skip it.
				continue;
			}

			// Match directive line into tokens with pattern of '\s+' (whitespaces).
			// We want to use this pattern to split the string into 3 tokens:
			//     i.e. @property [uniform_name] [display_name]
			std::regex directiveRegexPattern(R"(\s+)");
			std::vector<std::string> tokens;
			int numberOfMatches = 0;
			std::smatch matchObject;

			std::string lineToBeProcessed = line;
			while (std::regex_search(lineToBeProcessed, matchObject, directiveRegexPattern))
			{
				// Add the token
				tokens.emplace_back(lineToBeProcessed.substr(0, matchObject.position()));

				// Skip the matched string (whitespaces)
				lineToBeProcessed = lineToBeProcessed.substr(matchObject.position() + matchObject.length());

				numberOfMatches++;
				if (numberOfMatches >= 2)
				{
					// We only need at most 3 tokens,
					// so 2 matches would already give us 3 tokens.
					// Push the rest of the string as the final token.
					tokens.emplace_back(lineToBeProcessed);
					break;
				}
			}

			/*
			// -1 here means collect unmatched substrings. You could also put 0 in the list to collect matched substrings (i.e. whitespace).
			std::sregex_token_iterator tokenIterator(line.begin(), line.end(), directiveRegexPattern, {-1});
			std::remove_copy_if
				(
					tokenIterator,
					std::sregex_token_iterator(),
					std::back_inserter(tokens),
					[](std::string const &s) { return s.empty(); }
				);
			*/

			if (tokens.size() < 2)
			{
				// There should be at least two tokens in the line i.e. @property _Color
				// Otherwise we would ignore it.
				DYE_LOG("Property directive '%s' is presented but no uniform variable name follows. Property directive is ignored.", line.c_str());
				continue;
			}

			// The second token is the name of the target uniform.
			auto& targetUniformName = tokens[1];
			std::string displayName = targetUniformName; // By default, display name is the name of the uniform.

			const auto& existingProperties = shaderProgram.GetAllPropertyInfo();
			auto findDuplicatePropertyIterator = std::find_if
				(
					existingProperties.begin(),
					existingProperties.end(),
					[targetUniformName](const PropertyInfo& info)
					{
						return targetUniformName == info.UniformName;
					}
				);
			if (findDuplicatePropertyIterator != std::end(existingProperties))
			{
				// There has already been a property with the same target uniform. Skip this one!
				DYE_LOG("A Property targeting uniform '%s' has already been recorded. Property directive '%s' is ignored.", targetUniformName.c_str(), line.c_str());
				continue;
			}


			auto findUniformIterator = std::find_if
				(
					uniformInfos.begin(),
					uniformInfos.end(),
					[targetUniformName](const UniformInfo& info)
					{
						return targetUniformName == info.Name;
					}
				);

			if (findUniformIterator == std::end(uniformInfos))
			{
				// Couldn't find a uniform with the target name.
				DYE_LOG("Property '%s' is presented but there is no uniform with the name. Property directive is ignored. "
						"Uniforms could be omitted by the shader compiler if they are not being used in the Shader code.", targetUniformName.c_str());
				continue;
			}

			auto& uniformInfo = *findUniformIterator;
			if (tokens.size() >= 3)
			{
				// Third token exists, could potentially be the custom display name for the property.
				auto candidateDisplayName = tokens[2];
				std::regex displayNameRegexPattern(R"(".+")");
				bool isValidDisplayName = std::regex_match(candidateDisplayName, displayNameRegexPattern);
				if (isValidDisplayName)
				{
					// Remove the first and the last characters (\") in the string.
					displayName = candidateDisplayName.substr(1, candidateDisplayName.length() - 2);
				}
			}

			DYE_LOG("%s", line.c_str());
			shaderProgram.AddPropertyInfo(PropertyInfo { .Type = uniformInfo.Type, .UniformName = uniformInfo.Name, .DisplayName = displayName });
		}

		DYE_LOG("There are [%d] uniform properties.", shaderProgram.GetAllPropertyInfo().size());
	}
}
