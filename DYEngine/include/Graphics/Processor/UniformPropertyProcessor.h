#pragma once

#include "Graphics/ShaderProcessorBase.h"

namespace DYE::ShaderProcessor
{
	/// Process the uniform properties of the shader. Properties are uniforms exposed to users.
	/// Properties declared in Shader will be serialized into Material (namely Shader Instance Data).
	class UniformPropertyProcessor : public ShaderProcessorBase
	{
		constexpr static const char* UniformPropertyDirective = "@Property";

	public:
		UniformPropertyProcessor() : ShaderProcessorBase("Uniform Property Processor") {}

		std::vector<std::string> GetDirectivesToIgnoreInShaderTypeParsePhase() const override
		{
			return { UniformPropertyDirective };
		}

		void OnBegin(DYE::ShaderProgram &shaderProgram) override;
		void OnPreShaderTypeParse(std::string &programSource) override;
		void OnEnd(DYE::ShaderProgram &shaderProgram) override;
	private:
		std::vector<UniformInfo> getAllActiveUniformInfoFromShaderProgram(ShaderProgram& shaderProgram);

	private:
		std::string m_ShaderProgramSourceCache;
	};
}