#pragma once

#include "Graphics/ShaderProcessorBase.h"

namespace DYE::ShaderProcessor
{
	/// Process the uniform properties of the shader. Properties are uniforms exposed to users.
	///  Properties declared in Shader will be serialized into Material (namely Shader Instance Data).
	class UniformPropertyProcessor : public ShaderProcessorBase
	{
		constexpr static const char* UniformPropertySpecifier = "#property";

	public:
		UniformPropertyProcessor() : ShaderProcessorBase("Uniform Property Processor") {}
		void OnBegin(DYE::ShaderProgram &shaderProgram) override;
		void OnEnd(DYE::ShaderProgram &shaderProgram) override;
	};
}