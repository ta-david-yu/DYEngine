#include "Graphics/MaterialPropertyBlock.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DYE
{
	float MaterialPropertyBlock::GetFloat(const std::string& name) const
	{
		auto const& propertiesOfType = m_FloatProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return 0;
		}
		return result->second;
	}
	glm::vec2 MaterialPropertyBlock::GetFloat2(const std::string& name) const
	{
		auto const& propertiesOfType = m_Float2Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::vec3 MaterialPropertyBlock::GetFloat3(const std::string& name) const
	{
		auto const& propertiesOfType = m_Float3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::vec4 MaterialPropertyBlock::GetFloat4(const std::string& name) const
	{
		auto const& propertiesOfType = m_Float4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::mat3 MaterialPropertyBlock::GetMat3(const std::string& name) const
	{
		auto const& propertiesOfType = m_Mat3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::mat4 MaterialPropertyBlock::GetMat4(const std::string& name) const
	{
		auto const& propertiesOfType = m_Mat4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	int MaterialPropertyBlock::GetInt(const std::string& name) const
	{
		auto const& propertiesOfType = m_IntProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	bool MaterialPropertyBlock::GetBool(const std::string& name) const
	{
		auto const& propertiesOfType = m_IntProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	Texture* MaterialPropertyBlock::GetTexture(const std::string &name) const
	{
		auto const& propertiesOfType = m_TextureProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			return nullptr;
		}
		return result->second.get();
	}

	void MaterialPropertyBlock::SetFloat(const std::string& name, float value)
	{
		auto& propertiesOfType = m_FloatProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, value});
			return;
		}
		result->second = value;

	}
	void MaterialPropertyBlock::SetFloat2(const std::string& name, glm::vec2 value)
	{
		auto& propertiesOfType = m_Float2Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.emplace_back(name, value);
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetFloat3(const std::string& name, glm::vec3 value)
	{
		auto& propertiesOfType = m_Float3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.emplace_back(name, value);
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetFloat4(const std::string& name, glm::vec4 value)
	{
		auto& propertiesOfType = m_Float4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, value});
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetMat3(const std::string& name, glm::mat3 value)
	{
		auto& propertiesOfType = m_Mat3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, value});
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetMat4(const std::string& name, glm::mat4 value)
	{
		auto& propertiesOfType = m_Mat4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, value});
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetInt(const std::string& name, int value)
	{
		auto& propertiesOfType = m_IntProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, value});
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetBool(const std::string& name, bool value)
	{
		auto& propertiesOfType = m_BoolProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, value});
			return;
		}
		result->second = value;
	}
	void MaterialPropertyBlock::SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture)
	{
		auto& propertiesOfType = m_TextureProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first == name; });
		if (result == propertiesOfType.end())
		{
			propertiesOfType.push_back({name, texture});
			return;
		}
		result->second = texture;
	}

	void MaterialPropertyBlock::updatePropertyValuesToGPU(ShaderProgram const& shaderProgram) const
	{
		for (auto const& propertyPair : m_FloatProperties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniform1fv(location, 1, &propertyPair.second));
		}

		for (auto const& propertyPair : m_Float2Properties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniform2fv(location, 1, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Float3Properties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniform3fv(location, 1, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Float4Properties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniform4fv(location, 1, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Mat3Properties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Mat4Properties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_IntProperties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniform1iv(location, 1, &propertyPair.second));
		}

		for (auto const& propertyPair : m_BoolProperties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			UniformLocation const location = uniformInfo.value().Location;
			glCall(glUniform1iv(location, 1, &propertyPair.second));
		}

		for (auto const& propertyPair : m_TextureProperties)
		{
			auto const& uniformInfo = shaderProgram.TryGetUniformInfo(propertyPair.first);
			if (!uniformInfo.has_value())
			{
				continue;
			}

			propertyPair.second->Bind(uniformInfo.value().TextureUnitSlotIfTexture);
		}
	}
}