#include "Graphics/Processor/UniformPropertyProcessor.h"

#include "Util/Macro.h"
#include "Graphics/UniformType.h"
#include "Graphics/OpenGL.h"

#include <regex>

namespace DYE::ShaderProcessor
{
	void UniformPropertyProcessor::OnBegin(DYE::ShaderProgram &shaderProgram)
	{
	}

	void UniformPropertyProcessor::OnPreShaderTypeParse(std::string &programSource)
	{
		m_ShaderProgramSourceCache = programSource;
	}

	void UniformPropertyProcessor::OnEnd(DYE::ShaderProgram &shaderProgram)
	{
		// Acquire uniform information from shader program.
		auto uniformInfos = getAllActiveUniformInfoFromShaderProgram(shaderProgram);
		shaderProgram.addUniformInfo(uniformInfos);

		// Parse property information from cached shader program source.
		std::stringstream stream(m_ShaderProgramSourceCache);

		std::string line;
		while (std::getline(stream, line))
		{
			auto firstToken = getFirstTokenFromLine(line);
			if (firstToken != UniformPropertyDirective)
			{
				// We aren't interested in normal lines. Skip it.
				continue;
			}

			// We want to split the string into 3 tokens:
			// 		i.e. @property [uniform_name] [display_name]
			std::vector<std::string> const tokens = splitLineIntoTokensBySpace(line, 3);

			if (tokens.size() < 2)
			{
				// There should be at least two tokens in the line i.e. @property _Color. Otherwise we would ignore it.
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
				std::regex const displayNameRegexPattern(R"(".+")");
				bool const isValidDisplayName = std::regex_match(candidateDisplayName, displayNameRegexPattern);
				if (isValidDisplayName)
				{
					// Remove the first and the last characters (\") in the string.
					displayName = candidateDisplayName.substr(1, candidateDisplayName.length() - 2);
				}
			}

			shaderProgram.addPropertyInfo(
				PropertyInfo {.UniformName = uniformInfo.Name, .Type = uniformInfo.Type, .DisplayName = displayName});
		}

		DYE_LOG("There are [%d] uniform properties.", shaderProgram.GetAllPropertyInfo().size());
	}

	std::vector<UniformInfo> UniformPropertyProcessor::getAllActiveUniformInfoFromShaderProgram(ShaderProgram& shaderProgram)
	{
		/// Init vector.
		std::vector<UniformInfo> infos {};

		ShaderID const shaderId = shaderProgram.GetID();

		/// Parse uniform variables and cache the information.
		std::int32_t numberOfUniforms = 0;
		glGetProgramiv(shaderId, GL_ACTIVE_UNIFORMS, &numberOfUniforms);

		if (numberOfUniforms <= 0)
		{
			return {};
		}

		std::int32_t maxUniformNameLength = 0;
		UniformSize uniformNameLength = 0;
		UniformSize uniformSize = 0;
		GLUniformEnum uniformType = GL_NONE;

		glGetProgramiv(shaderProgram.GetID(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		auto uniformName = std::make_unique<char[]>(maxUniformNameLength);

		/// Use this program before reading/writing uniforms.
		shaderProgram.Use();

		int textureUnitSlotCounter = 0;
		for (std::int32_t i = 0; i < numberOfUniforms; i++)
		{
			glGetActiveUniform(shaderId, i, maxUniformNameLength, &uniformNameLength, &uniformSize, &uniformType,
							   uniformName.get());

			UniformInfo info{};
			info.Name = std::string(uniformName.get(), uniformNameLength);
			info.Type = GLTypeToUniformType(uniformType);
			info.Location = glGetUniformLocation(shaderId, uniformName.get());

			if (info.Type == UniformType::Texture2D)
			{
				// If the variable is a texture,
				// we want to bind texture unit slot to the uniform.
				glCall(glUniform1i(info.Location, textureUnitSlotCounter));
				info.TextureUnitSlotIfTexture = textureUnitSlotCounter;

				textureUnitSlotCounter++;
			}
			// TODO: add more texture type here (sampler)

			infos.push_back(info);
		}

		DYE_LOG("There are [%d] uniform variables.", infos.size());

		return std::move(infos);
	}
}
