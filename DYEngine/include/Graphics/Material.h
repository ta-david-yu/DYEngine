#pragma once

#include "Graphics/UniformType.h"

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <optional>

#include <glm/glm.hpp>

namespace DYE
{
	class Texture;
	class ShaderProgram;
	class RenderCommand;

	class Material
	{
		friend RenderCommand;

	public:
		Material(std::string name, const std::shared_ptr<ShaderProgram>& shaderProgram);
		~Material() = default;

		/// A factory function that creates a material from a shader program
		/// \param shader
		/// \return a shared pointer to the newly created Material, return shared_ptr(nullptr) if failed
		static std::shared_ptr<Material> CreateFromShader(const std::string& name, std::shared_ptr<ShaderProgram> const& shader);

		std::string GetName() const { return m_Name; }
		ShaderProgram& GetShaderProgram() const { return *m_Shader; }

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
		// Update the values of uniform variables based on property values.
		// Called during RenderCommand to bind values to shader program.
		void updateUniformValuesToGPU() const;

	private:
		std::shared_ptr<ShaderProgram> m_Shader;

	// Data to be serialized goes below here :)) for now
	private:
		// The name of the Material (debugging purpose)
		std::string m_Name;

		// Store the file path to the shader program file this material is using.
		// This would be empty if the Material is created from loaded ShaderProgram instance.
		std::string m_ShaderFilePath;

		std::vector<std::pair<UniformInfo, GLfloat>> m_FloatProperties {};
		std::vector<std::pair<UniformInfo, glm::vec2>> m_Float2Properties {};
		std::vector<std::pair<UniformInfo, glm::vec3>> m_Float3Properties {};
		std::vector<std::pair<UniformInfo, glm::vec4>> m_Float4Properties {};
		std::vector<std::pair<UniformInfo, glm::mat3>> m_Mat3Properties {};
		std::vector<std::pair<UniformInfo, glm::mat4>> m_Mat4Properties {};
		std::vector<std::pair<UniformInfo, GLint>> m_IntProperties {};
		std::vector<std::pair<UniformInfo, GLint>> m_BoolProperties {};
		std::vector<std::pair<UniformInfo, std::shared_ptr<Texture>>> m_TextureProperties {};
	};
}