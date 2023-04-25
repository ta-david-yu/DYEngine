#include "Graphics/Texture.h"
#include "Graphics/OpenGL.h"
#include "Util/Macro.h"

#include <stb_image.h>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <array>

namespace DYE
{
	void Texture::SetWrapMode(WrapMode wrapMode)
	{
		// TODO: add per axis (u, v) settings.

		auto id = GetID();
		switch (wrapMode)
		{
			case WrapMode::Repeat:
				glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			case WrapMode::Clamp:
				glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP);
				break;
		}
	}

	void Texture::SetFilterMode(FilterMode filterMode)
	{
		// TODO: add per min/mag filter settings.

		auto id = GetID();
		switch (filterMode)
		{
			case FilterMode::Nearest:
				glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case FilterMode::Linear:
				glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
		}
	}

    std::shared_ptr<Texture2D> Texture2D::Create(std::uint32_t width, std::uint32_t height)
    {
        return std::make_shared<Texture2D>(width, height);
    }

    std::shared_ptr<Texture2D> Texture2D::Create(glm::vec4 color)
    {
        unsigned char data[4] = { static_cast<unsigned char>(color.r * 255),
                                  static_cast<unsigned char>(color.g * 255),
                                  static_cast<unsigned char>(color.b * 255),
                                  static_cast<unsigned char>(color.a * 255)};

        auto texture = std::make_shared<Texture2D>(1, 1);
        texture->SetData((void*) data, 1);
        return std::move(texture);
    }

	std::shared_ptr<Texture2D> Texture2D::Create(glm::vec4 color, std::uint32_t width, std::uint32_t height)
	{
		// Create texture data dynamically.
		std::uint32_t const numberOfPixels = width * height;
		std::vector data
		{
			numberOfPixels,

			// array of color components (R, G, B, A)
			std::array<unsigned char, 4>
			    {
					static_cast<unsigned char>(color.r * 255),
					static_cast<unsigned char>(color.g * 255),
					static_cast<unsigned char>(color.b * 255),
					static_cast<unsigned char>(color.a * 255)
				}
		};

  		auto texture = std::make_shared<Texture2D>(width, height);
		texture->SetData(data.data(), numberOfPixels);

		return std::move(texture);
	}

    std::shared_ptr<Texture2D> Texture2D::Create(const std::filesystem::path& path)
    {
        return std::make_shared<Texture2D>(path);
    }

	std::shared_ptr<Texture2D> Texture2D::GetWhiteTexture()
	{
		static std::shared_ptr<Texture2D> const whiteTexture = Create(glm::vec4{1, 1, 1, 1});
		return whiteTexture;
	}

	std::shared_ptr<Texture2D> Texture2D::GetDefaultTexture()
	{
		return GetWhiteTexture();
	}

    Texture2D::Texture2D(std::uint32_t width, std::uint32_t height)
        : m_Width(width), m_Height(height)
    {
        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        glCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_ID));
        DYE_LOG("Create Texture (%d), %d x %d", m_ID, width, height);
        glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

#ifdef DYE_DEBUG
        // apply the name, -1 means NULL terminated
        glObjectLabel(GL_TEXTURE, m_ID, -1, "WxH");
#endif
    }

    Texture2D::Texture2D(const std::filesystem::path& path) : m_Path(path)
    {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);

        if (data != nullptr)
        {
            m_Width = width;
            m_Height = height;
        }
        else
        {
            DYE_LOG("Failed to load texture \"%s\"!", path.string().c_str());
            DYE_ASSERT(false);
        }

		if (channels == 4)
		{
            m_InternalFormat = GL_RGBA8;
            m_DataFormat = GL_RGBA;
        }
		else if (channels == 3)
        {
            m_InternalFormat = GL_RGB8;
            m_DataFormat = GL_RGB;
        }
		else if (channels == 2)
		{
			m_InternalFormat = GL_RGBA8;
			m_DataFormat = GL_RGBA;
		}

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);
        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, (void*)data);

		// Check if the filters are set correctly
		int minFilter, magFilter;
		glGetTextureParameteriv(m_ID, GL_TEXTURE_MIN_FILTER, &minFilter);
		glGetTextureParameteriv(m_ID, GL_TEXTURE_MAG_FILTER, &magFilter);

        DYE_LOG("Create texture (%d) from \"%s\"\n\tComponents - %d\n\tDimension - %d x %d\n\tMin Filter - %#08x\n\tMag Filter - %#08x",
				m_ID,
				m_Path.string().c_str(),
                channels,
                m_Width,
                m_Height,
				minFilter,
				magFilter);

#ifdef DYE_DEBUG
        // apply the name, -1 means NULL terminated
        glObjectLabel(GL_TEXTURE, m_ID, -1, m_Path.string().c_str());
#endif

        stbi_image_free(data);
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_ID);
    }

	glm::mat4 Texture2D::GetScaleMatrixFromTextureDimensions() const
	{
		glm::vec3 scale { (float) m_Width / PixelsPerUnit, (float) m_Height / PixelsPerUnit, 1 };
		return glm::scale(glm::mat4{1}, scale);
	}

    void Texture2D::SetData(void *data, std::uint32_t size)
    {
        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

	/// Bind this texture to the given texture unit location.
	/// \param textureUnitSlot texture unit location
    void Texture2D::Bind(std::uint32_t textureUnitSlot)
    {
        glBindTextureUnit(textureUnitSlot, m_ID);
    }
}