#include "Graphics/Material.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DYE
{
	std::shared_ptr<Material> Material::CreateFromShader(const std::string& name, std::shared_ptr<ShaderProgram> const& shader)
	{
		DYE_LOG("<< Start creating material \"%s\" from shader \"%s\" >>", name.c_str(), shader->GetName().c_str());

		auto material = std::make_shared<Material>(name, shader);
		for (const auto& propertyInfo : shader->GetAllPropertyInfo())
		{
			std::optional<UniformInfo> const uniformInfo = shader->TryGetUniformInfo(propertyInfo.UniformName);

			if (!uniformInfo.has_value())
			{
				continue;
			}

			switch (propertyInfo.Type)
			{
				case UniformType::Float:
					material->m_FloatProperties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Float2:
					material->m_Float2Properties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Float3:
					material->m_Float3Properties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Float4:
					material->m_Float4Properties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Mat3:
					material->m_Mat3Properties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Mat4:
					material->m_Mat4Properties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Int:
					material->m_IntProperties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Boolean:
					material->m_BoolProperties.emplace_back(uniformInfo.value(), 0);
					break;
				case UniformType::Texture2D:
					material->m_TextureProperties.emplace_back(uniformInfo.value(), Texture2D::GetDefaultTexture());
					break;
				case UniformType::Invalid:
					break;
			}
		}

		return std::move(material);
	}

	Material::Material(std::string name, const std::shared_ptr<ShaderProgram>& shaderProgram) : m_Name(std::move(name)), m_Shader(shaderProgram)
	{
	}

	float Material::GetFloat(const std::string& name) const
	{
		auto const& propertiesOfType = m_FloatProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return 0;
		}
		return result->second;
	}
	glm::vec2 Material::GetFloat2(const std::string& name) const
	{
		auto const& propertiesOfType = m_Float2Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::vec3 Material::GetFloat3(const std::string& name) const
	{
		auto const& propertiesOfType = m_Float3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::vec4 Material::GetFloat4(const std::string& name) const
	{
		auto const& propertiesOfType = m_Float4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::mat3 Material::GetMat3(const std::string& name) const
	{
		auto const& propertiesOfType = m_Mat3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	glm::mat4 Material::GetMat4(const std::string& name) const
	{
		auto const& propertiesOfType = m_Mat4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	int Material::GetInt(const std::string& name) const
	{
		auto const& propertiesOfType = m_IntProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	bool Material::GetBool(const std::string& name) const
	{
		auto const& propertiesOfType = m_IntProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return {};
		}
		return result->second;
	}
	Texture* Material::GetTexture(const std::string &name) const
	{
		auto const& propertiesOfType = m_TextureProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			return nullptr;
		}
		return result->second.get();
	}

	void Material::SetFloat(const std::string& name, float value)
	{
		auto& propertiesOfType = m_FloatProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;

	}
	void Material::SetFloat2(const std::string& name, glm::vec2 value)
	{
		auto& propertiesOfType = m_Float2Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetFloat3(const std::string& name, glm::vec3 value)
	{
		auto& propertiesOfType = m_Float3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetFloat4(const std::string& name, glm::vec4 value)
	{
		auto& propertiesOfType = m_Float4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetMat3(const std::string& name, glm::mat3 value)
	{
		auto& propertiesOfType = m_Mat3Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetMat4(const std::string& name, glm::mat4 value)
	{
		auto& propertiesOfType = m_Mat4Properties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetInt(const std::string& name, int value)
	{
		auto& propertiesOfType = m_IntProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetBool(const std::string& name, bool value)
	{
		auto& propertiesOfType = m_BoolProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = value;
	}
	void Material::SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture)
	{
		auto& propertiesOfType = m_TextureProperties;
		auto result = std::ranges::find_if(propertiesOfType, [&](auto const& pair) { return pair.first.Name == name; });
		if (result == propertiesOfType.end())
		{
			DYE_LOG_WARN("Property '%s' does not exist in the Material '%s', set value function ignored!", name.c_str(), m_Name.c_str());
			return;
		}
		result->second = texture;
	}

	void Material::updateUniformValuesToGPU() const
	{
		ShaderProgram const& shaderProgram = GetShaderProgram();

		for (auto const& propertyPair : m_FloatProperties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniform1fv(location, 1, &propertyPair.second));
		}

		for (auto const& propertyPair : m_Float2Properties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniform2fv(location, 1, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Float3Properties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniform3fv(location, 1, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Float4Properties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniform4fv(location, 1, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Mat3Properties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_Mat4Properties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(propertyPair.second)));
		}

		for (auto const& propertyPair : m_IntProperties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniform1iv(location, 1, &propertyPair.second));
		}

		for (auto const& propertyPair : m_BoolProperties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			UniformLocation const location = uniformInfo.Location;
			glCall(glUniform1iv(location, 1, &propertyPair.second));
		}

		for (auto const& propertyPair : m_TextureProperties)
		{
			UniformInfo const& uniformInfo = propertyPair.first;
			if (!shaderProgram.HasUniform(uniformInfo.Name))
			{
				continue;
			}

			propertyPair.second->Bind(uniformInfo.TextureUnitSlotIfTexture);
		}
	}
}