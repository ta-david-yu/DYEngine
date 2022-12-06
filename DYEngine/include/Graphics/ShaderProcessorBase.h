#pragma once

#include "Graphics/Shader.h"

#include <vector>
#include <string>

namespace DYE
{
	class ShaderProcessorBase
	{
	public:
		explicit ShaderProcessorBase(std::string name);
		virtual ~ShaderProcessorBase() = default;

		std::string GetName() const { return m_Name; }

		/// Specify what directives should be ignored in the shader type parsing phase.
		/// For instance, UniformProperty directive #property should be omitted for the shader source.
		/// \return a list of c-strings to ignored
		virtual std::vector<std::string> GetDirectivesToIgnoreInShaderTypeParsePhase() const
		{
			// By default, it's empty.
			return { };
		};

		virtual void OnBegin(ShaderProgram& shaderProgram) = 0;
		virtual void OnPreShaderTypeParse(std::string& programSource) {};
		virtual void OnPostShaderTypeParse(ShaderProgram::ShaderTypeParseResult& parseResult) {};
		virtual void OnPreShaderCompilation(ShaderType shaderType, std::string& shaderSource) {};
		virtual void OnPostShaderCompilation(ShaderType shaderType, ShaderProgram::ShaderCompilationResult& compilationResult) {};
		virtual void OnEnd(ShaderProgram& shaderProgram) = 0;

	protected:
		static std::string getFirstTokenFromLine(const std::string& line)
		{
			auto firstToken = line.substr(0, line.find(' '));
			return firstToken;
		}

	private:
		std::string m_Name;
	};
}
