#pragma once

#include "Graphics/ShaderProcessorBase.h"
#include "Graphics/RenderState.h"

namespace DYE::ShaderProcessor
{
	/// Process the uniform properties of the shader. Properties are uniforms exposed to users.
	/// Properties declared in Shader will be serialized into Material (namely Shader Instance Data).
	class BlendStateCommandProcessor : public ShaderProcessorBase
	{
		constexpr static const char* BlendFunctionCommandDirective = "@blendFunc";
		constexpr static const char* BlendOperationCommandDirective = "@blendOp";

	public:
		BlendStateCommandProcessor() : ShaderProcessorBase("BlendState Command Processor") {}

		std::vector<std::string> GetDirectivesToIgnoreInShaderTypeParsePhase() const override
		{
			return { BlendFunctionCommandDirective, BlendOperationCommandDirective };
		}

		void OnBegin(DYE::ShaderProgram &shaderProgram) override;
		void OnPreShaderTypeParse(std::string &programSource) override;
		void OnEnd(DYE::ShaderProgram &shaderProgram) override;

	private:
		static BlendState parseBlendFunctionLine(std::string const& line);
		static void parseBlendOperationLine(std::string const& line);

		static std::optional<BlendState::BlendFactor> stringToBlendFactor(std::string const& input);
	private:
		std::string m_ShaderProgramSourceCache;
	};
}