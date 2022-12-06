#pragma once

#include "Graphics/Shader.h"

#include <string>

namespace DYE
{
	class ShaderProcessorBase
	{
	public:
		explicit ShaderProcessorBase(std::string name);
		virtual ~ShaderProcessorBase() = default;

		virtual void OnBegin(ShaderProgram& shaderProgram) = 0;
		virtual void OnPreShaderTypeParse(std::string& programSource) {};
		virtual void OnPostShaderTypeParse(ShaderProgram::ShaderTypeParseResult& parseResult) {};
		virtual void OnPreShaderCompilation(ShaderType shaderType, std::string& shaderSource) {};
		virtual void OnPostShaderCompilation(ShaderType shaderType, ShaderProgram::ShaderCompilationResult& compilationResult) {};
		virtual void OnEnd(ShaderProgram& shaderProgram) = 0;

		std::string GetName() const { return m_Name; }

	private:
		std::string m_Name;
	};
}
