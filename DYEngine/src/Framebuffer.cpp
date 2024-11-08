#include <utility>

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

    // Utility functions
    namespace Util
    {
        bool IsDepthFormat(FramebufferTextureFormat format)
        {
            switch (format)
            {
                // TODO: add more depth formats
                case FramebufferTextureFormat::Depth24Stencil8:
                    return true;
            }

            return false;
        }
    }

    Framebuffer::Framebuffer(FramebufferProperties properties) : m_Properties(std::move(properties))
    {
        for (FramebufferTextureProperties &textureProperties: m_Properties.Attachments.TexturePropertiesList)
        {
            if (!Util::IsDepthFormat(textureProperties.TextureFormat))
            {
                m_ColorAttachmentPropertiesList.emplace_back(textureProperties);
            }
            else
            {
                m_DepthAttachmentProperties = textureProperties;
            }
        }

        CreateOrReset();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_ID);
        glDeleteTextures(m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());
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
        DYE_ASSERT_LOG_WARN(width != 0 && height != 0 && width <= maxDimensions.x && height <= maxDimensions.y,
                            "Framebuffer::Resize: attempted to resize framebuffer [%d] to %d, %d.", m_ID, width, height);

        m_Properties.Width = width;
        m_Properties.Height = height;
        CreateOrReset();
    }

    void Framebuffer::CreateOrReset()
    {
        DYE_ASSERT_LOG_WARN(m_ColorAttachmentPropertiesList.size() <= 4,
                            "You can only have at max 4 color attachments but %zu are provided.",
                            m_ColorAttachmentPropertiesList.size());

        bool const isCreated = m_ID != 0;
        if (isCreated)
        {
            glDeleteFramebuffers(1, &m_ID);
            glDeleteTextures(m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());
            glDeleteTextures(1, &m_DepthStencilAttachmentID);
        }

        glCreateFramebuffers(1, &m_ID);

        bool const multiSampled = m_Properties.Samples > 1;

        if (!m_ColorAttachmentPropertiesList.empty())
        {
            m_ColorAttachmentIDs.resize(m_ColorAttachmentPropertiesList.size());
            glCreateTextures(multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());

            // Color textures initialization & attachments.
            // TODO: later we might want to create a color texture 2d handle so others could access the color attachment as if it's
            // 		a texture 2d.
            for (std::size_t i = 0; i < m_ColorAttachmentIDs.size(); i++)
            {
                TextureID colorAttachmentID = m_ColorAttachmentIDs[i];
                FramebufferTextureFormat textureFormat = m_ColorAttachmentPropertiesList[i].TextureFormat;

                GLenum glTextureFormat;
                switch (textureFormat)
                {
                    // TODO: add more framebuffer color texture formats
                    case FramebufferTextureFormat::RGBA8:
                        glTextureFormat = GL_RGBA8;
                        break;
                    case FramebufferTextureFormat::RedInteger:
                        glTextureFormat = GL_R32I;
                        break;
                }

                if (!multiSampled)
                {
                    glTextureStorage2D(colorAttachmentID, 1, glTextureFormat, m_Properties.Width, m_Properties.Height);
                }
                else
                {
                    glTextureStorage2DMultisample(colorAttachmentID, m_Properties.Samples, glTextureFormat, m_Properties.Width, m_Properties.Height, GL_FALSE);
                }

                // TODO: set different value based on filter mode & wrap mode properties
                glTextureParameteri(colorAttachmentID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(colorAttachmentID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(colorAttachmentID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTextureParameteri(colorAttachmentID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(colorAttachmentID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + i, colorAttachmentID, 0);
            }
        }

        // Depth & stencil texture initialization & attachment.
        if (m_DepthAttachmentProperties.TextureFormat != FramebufferTextureFormat::None)
        {
            glCreateTextures(multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &m_DepthStencilAttachmentID);

            GLenum glTextureFormat;
            switch (m_DepthAttachmentProperties.TextureFormat)
            {
                // TODO: add more framebuffer depth texture formats
                case FramebufferTextureFormat::Depth24Stencil8:
                    glTextureFormat = GL_DEPTH24_STENCIL8;
                    break;
            }

            glTextureStorage2D(m_DepthStencilAttachmentID, 1, glTextureFormat, m_Properties.Width, m_Properties.Height);

            glNamedFramebufferTexture(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthStencilAttachmentID, 0);
        }

        if (m_ColorAttachmentIDs.size() > 1)
        {
            // If there are more than 1 color attachments, we need to direct the fragment outputs
            // to the additional color attachments.
            GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT2};
            glNamedFramebufferDrawBuffers(m_ID, m_ColorAttachmentIDs.size(), buffers);
        }
        else if (m_ColorAttachmentIDs.empty())
        {
            // If no color attachment, we don't need to direct fragment color output.
            glNamedFramebufferDrawBuffer(m_ID, GL_NONE);
        }

        auto status = glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER);
        DYE_ASSERT_LOG_WARN(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer %d is incomplete.", m_ID);

#ifdef DYE_DEBUG
        if (!m_DebugName.empty())
        {
            glObjectLabel(GL_FRAMEBUFFER, m_ID, -1, m_DebugName.c_str());
        }
#endif
    }


    int Framebuffer::ReadPixelAsInteger(std::uint32_t colorAttachmentIndex, int x, int y)
    {
        DYE_ASSERT_LOG_WARN(colorAttachmentIndex < m_ColorAttachmentIDs.size(),
                            "Trying to read pixel from attachment index %d but there are only %zu attachments",
                            colorAttachmentIndex,
                            m_ColorAttachmentIDs.size());


        // FIXME: glNamedFramebufferReadBuffer for some reasons doesn't work :P
        //	 so we need to do it the old-school way, binding/unbinding manually.
        //glNamedFramebufferReadBuffer(m_ID, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);

        glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
        int pixelData = -1;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return pixelData;
    }

    void Framebuffer::ClearAttachment(std::uint32_t colorAttachmentIndex, int value)
    {
        DYE_ASSERT_LOG_WARN(colorAttachmentIndex < m_ColorAttachmentIDs.size(),
                            "Trying to clear color attachment index %d but there are only %zu attachments",
                            colorAttachmentIndex,
                            m_ColorAttachmentIDs.size());

        FramebufferTextureFormat textureFormat = m_ColorAttachmentPropertiesList[colorAttachmentIndex].TextureFormat;
        GLenum glTextureFormat;
        switch (textureFormat)
        {
            // TODO: add more framebuffer color texture formats
            case FramebufferTextureFormat::RGBA8:
                glTextureFormat = GL_RGBA8;
                break;
            case FramebufferTextureFormat::RedInteger:
                glTextureFormat = GL_RED_INTEGER;
                break;
        }

        FramebufferTextureProperties &properties = m_ColorAttachmentPropertiesList[colorAttachmentIndex];
        glClearTexImage(m_ColorAttachmentIDs[colorAttachmentIndex], 0, glTextureFormat, GL_INT, &value);
    }

    TextureID Framebuffer::GetColorAttachmentID(std::size_t index) const
    {
        DYE_ASSERT_LOG_WARN(index < m_ColorAttachmentIDs.size(),
                            "Trying to access color attachment index %d but there are only %zu.",
                            m_ColorAttachmentIDs.size(),
                            index);
        return m_ColorAttachmentIDs[index];
    }

    void Framebuffer::SetDebugLabel(std::string const &name)
    {
#ifdef DYE_DEBUG
        m_DebugName = name;
        glObjectLabel(GL_FRAMEBUFFER, m_ID, -1, name.c_str());
#endif
    }
}