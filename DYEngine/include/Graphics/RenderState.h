#pragma once

#include "Graphics/OpenGL.h"

#include <optional>

namespace DYE
{
	enum class CompareFunction
	{
		Never = GL_NEVER,
		Less = GL_LESS,
		Equal = GL_EQUAL,
		LessEqual = GL_LEQUAL,
		Greater = GL_GREATER,
		NotEqual = GL_NOTEQUAL,
		GreaterEqual = GL_GEQUAL,
		Always = GL_ALWAYS
	};

	std::optional<CompareFunction> StringToCompareFunction(std::string const& input);

	struct DepthState
	{
		bool IsEnabled = true;
		bool IsWriteEnabled = true;
		CompareFunction CompareFunction = CompareFunction::Less;
	};

	struct BlendState
	{
		enum class BlendFactor
		{
			Zero = GL_ZERO,
			One = GL_ONE,
			SrcColor = GL_SRC_COLOR,
			OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
			SrcAlpha = GL_SRC_ALPHA,
			OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
			DstAlpha = GL_DST_ALPHA,
			OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
			DstColor = GL_DST_COLOR,
			OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
		};

		bool IsEnabled = false;
		BlendFactor SourceFactor = BlendFactor::SrcAlpha;
		BlendFactor DestinationFactor = BlendFactor::OneMinusSrcAlpha;

		// TODO: Add more option such as BlendOp etc
	};

	struct RenderState
	{
	public:
		DepthState DepthState;
		BlendState BlendState;
		// TODO: Add more command such as Stencil Test, Cull etc

		void Apply();

	private:
		void applyDepthState();
		void applyBlendState();
	};
}