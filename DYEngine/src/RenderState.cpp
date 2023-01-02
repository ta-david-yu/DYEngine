#include "Graphics/RenderState.h"

namespace DYE
{

	std::optional<CompareFunction> StringToCompareFunction(std::string const& input)
	{
		if (input == "Never")
		{
			return CompareFunction::Never;
		}

		if (input == "Less")
		{
			return CompareFunction::Less;
		}

		if (input == "Equal")
		{
			return CompareFunction::Equal;
		}

		if (input == "LessEqual" || input == "LEqual")
		{
			return CompareFunction::LessEqual;
		}

		if (input == "Greater")
		{
			return CompareFunction::Greater;
		}

		if (input == "NotEqual")
		{
			return CompareFunction::NotEqual;
		}

		if (input == "GreaterEqual" || input == "GEqual")
		{
			return CompareFunction::GreaterEqual;
		}

		if (input == "Always")
		{
			return CompareFunction::Always;
		}

		return {};
	}

	void RenderState::Apply()
	{
		applyDepthState();
		applyBlendState();
	}

	void RenderState::applyDepthState()
	{
		if (!DepthState.IsEnabled)
		{
			glCall(glDisable(GL_DEPTH_TEST));
			return;
		}

		glCall(glEnable(GL_DEPTH_TEST));

		// Set ZWrite
		if (DepthState.IsWriteEnabled)
		{
			glCall(glDepthMask(GL_TRUE));
		}
		else
		{
			glCall(glDepthMask(GL_FALSE));
		}

		// Set ZTest
		glCall(glDepthFunc(static_cast<GLenum>(DepthState.CompareFunction)));
	}

	void RenderState::applyBlendState()
	{
		if (!BlendState.IsEnabled)
		{
			glCall(glDisable(GL_BLEND));
			return;
		}

		glCall(glEnable(GL_BLEND));

		// Set Blend Func
		glCall(glBlendFunc(
			static_cast<GLenum>(BlendState.SourceFactor),
			static_cast<GLenum>(BlendState.DestinationFactor)));
	}
}