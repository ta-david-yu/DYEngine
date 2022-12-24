#include "Graphics/RenderState.h"

namespace DYE
{

	void RenderState::Apply()
	{
		// TODO: set depth command
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