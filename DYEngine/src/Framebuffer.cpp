#include "Graphics/Framebuffer.h"

#include "Graphics/OpenGL.h"
#include "Graphics/RenderCommand.h"
#include "Util/Logger.h"
#include "Util/Macro.h"

namespace DYE
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(const DYE::FramebufferProperties &properties)
	{
		return std::make_shared<Framebuffer>(properties);
	}

	Framebuffer::Framebuffer(const FramebufferProperties &properties) : m_Properties(properties)
	{
		CreateOrReset();
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
		glDeleteTextures(1, &m_ColorAttachmentID);
		glDeleteTextures(1, &m_DepthStencilAttachmentID);
	}

	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	}

	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(std::uint32_t width, std::uint32_t height)
	{
		auto maxDimensions = RenderCommand::GetInstance().GetMaxFramebufferSize();
		DYE_ASSERT_LOG(width != 0 && height != 0 && width <= maxDimensions.x && height <= maxDimensions.y,
					   "Framebuffer::Resize: attempted to resize framebuffer [%d] to %d, %d.", m_ID, width, height);

		m_Properties.Width = width;
		m_Properties.Height = height;
		CreateOrReset();
	}

	void Framebuffer::CreateOrReset()
	{
		bool const isCreated = m_ID != 0;
		if (isCreated)
		{
			glDeleteFramebuffers(1, &m_ID);
			glDeleteTextures(1, &m_ColorAttachmentID);
			glDeleteTextures(1, &m_DepthStencilAttachmentID);
		}

		glCreateFramebuffers(1, &m_ID);

		// Create a color attachment (texture 2d) for the framebuffer.
		// TODO: later we want to create a color texture 2d handle so others could access the color attachment as if it's
		// 		a texture 2d.
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentID);
		glTextureStorage2D(m_ColorAttachmentID, 1, GL_RGBA8, m_Properties.Width, m_Properties.Height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0, m_ColorAttachmentID, 0);

		// Create depth & stencil texture for the framebuffer.
		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthStencilAttachmentID);
		glTextureStorage2D(m_DepthStencilAttachmentID, 1, GL_DEPTH24_STENCIL8, m_Properties.Width, m_Properties.Height);

		glNamedFramebufferTexture(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthStencilAttachmentID, 0);

		auto status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
		DYE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE && "Framebuffer is incomplete.");

	}
}