#include "Graphics/Processor/UniformPropertyProcessor.h"

#include "Base.h"
#include "Graphics/UniformType.h"

namespace DYE::ShaderProcessor
{
	void UniformPropertyProcessor::OnBegin(DYE::ShaderProgram &shaderProgram)
	{
		UniformType type;
		//DYE_ASSERT(HasValidPropertyTypeKeywordInLine("#property sampler1D Matrix4x4", type));
		//DYE_LOG(UniformTypeToPropertyTypeQualifier(UniformType::Boolean).c_str());
	}

	void UniformPropertyProcessor::OnEnd(DYE::ShaderProgram &shaderProgram)
	{

	}
}
