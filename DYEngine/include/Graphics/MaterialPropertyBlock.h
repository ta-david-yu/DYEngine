#pragma once

#include "Graphics/UniformType.h"

#include <string>
#include <memory>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

namespace DYE
{
	class Texture;
	class ShaderProgram;
	class RenderCommand;

	class MaterialPropertyBlock
	{
		friend RenderCommand;
	public:
		float GetFloat(const std::string& name) const;
		glm::vec2 GetFloat2(const std::string& name) const;
		glm::vec3 GetFloat3(const std::string& name) const;
		glm::vec4 GetFloat4(const std::string& name) const;
		glm::mat3 GetMat3(const std::string& name) const;
		glm::mat4 GetMat4(const std::string& name) const;
		int GetInt(const std::string& name) const;
		bool GetBool(const std::string& name) const;
		Texture* GetTexture(const std::string& name) const;

		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, glm::vec2 value);
		void SetFloat3(const std::string& name, glm::vec3 value);
		void SetFloat4(const std::string& name, glm::vec4 value);
		void SetMat3(const std::string& name, glm::mat3 value);
		void SetMat4(const std::string& name, glm::mat4 value);
		void SetInt(const std::string& name, int value);
		void SetBool(const std::string& name, bool value);
		void SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture);

	private:
		// Update the values of properties for the given shader.
		// Called in RenderCommand to bind values to shader program.
		void updatePropertyValuesToGPU(ShaderProgram const& shaderProgram) const;

	private:
		std::vector<std::pair<std::string, GLfloat>> m_FloatProperties {};
		std::vector<std::pair<std::string, glm::vec2>> m_Float2Properties {};
		std::vector<std::pair<std::string, glm::vec3>> m_Float3Properties {};
		std::vector<std::pair<std::string, glm::vec4>> m_Float4Properties {};
		std::vector<std::pair<std::string, glm::mat3>> m_Mat3Properties {};
		std::vector<std::pair<std::string, glm::mat4>> m_Mat4Properties {};
		std::vector<std::pair<std::string, GLint>> m_IntProperties {};
		std::vector<std::pair<std::string, GLint>> m_BoolProperties {};
		std::vector<std::pair<std::string, std::shared_ptr<Texture>>> m_TextureProperties {};
	};
}