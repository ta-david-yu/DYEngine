#pragma once

#include "Graphics/ShaderProcessorBase.h"
#include "Graphics/RenderState.h"

namespace DYE::ShaderProcessor
{
	/// Process the uniform properties of the shader. Properties are uniforms exposed to users.
	/// Properties declared in Shader will be serialized into Material (namely Shader Instance Data).
	class DepthStateCommandProcessor : public ShaderProcessorBase
	{
		constexpr static const char* ZWriteCommandDirective = "@zWrite";
		constexpr static const char* ZTestCommandDirective = "@zTest";

	public:
		DepthStateCommandProcessor() : ShaderProcessorBase("DepthState Command Processor") {}

		std::vector<std::string> GetDirectivesToIgnoreInShaderTypeParsePhase() const override
		{
			return { ZWriteCommandDirective, ZTestCommandDirective };
		}

		void OnBegin(DYE::ShaderProgram &shaderProgram) override;
		void OnPreShaderTypeParse(std::string &programSource) override;
		void OnEnd(DYE::ShaderProgram &shaderProgram) override;

	private:
		static CompareFunction parseZTestLine(std::string const& line);
		static bool parseZWriteLine(std::string const& line);
	private:
		DepthState m_DepthStateCache;
	};
}
