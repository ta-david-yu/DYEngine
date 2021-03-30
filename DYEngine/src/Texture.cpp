#include "Graphics/Texture.h"
#include "Graphics/OpenGL.h"
#include "Base.h"

#include <stb_image.h>
#include <SDL_image.h>

namespace DYE
{
    /// Temporary flip SDL_Surface code
    void flip_surface(SDL_Surface* surface)
    {
        SDL_LockSurface(surface);

        int pitch = surface->pitch; // row size
        char* temp = new char[pitch]; // intermediate buffer
        char* pixels = (char*) surface->pixels;

        for(int i = 0; i < surface->h / 2; ++i) {
            // get pointers to the two rows to swap
            char* row1 = pixels + i * pitch;
            char* row2 = pixels + (surface->h - i - 1) * pitch;

            // swap rows
            memcpy(temp, row1, pitch);
            memcpy(row1, row2, pitch);
            memcpy(row2, temp, pitch);
        }

        delete[] temp;

        SDL_UnlockSurface(surface);
    }

    std::shared_ptr<Texture2D> Texture2D::Create(std::uint32_t width, std::uint32_t height)
    {
        return std::make_shared<Texture2D>(width, height);
    }

    std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
    {
        return std::make_shared<Texture2D>(path);
    }

    Texture2D::Texture2D(std::uint32_t width, std::uint32_t height)
        : m_Width(width), m_Height(height)
    {
        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        glCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_ID));
        DYE_LOG("Create Texture [%d], %d x %d", m_ID, width, height);
        glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

    Texture2D::Texture2D(const std::string &path) : m_Path(path)
    {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (data != nullptr)
        {
            m_Width = width;
            m_Height = height;
        }
        else
        {
            DYE_LOG("Failed to load texture %s!", path.c_str());
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

        glCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_ID));
        glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

        DYE_LOG("Create Texture [%d] from %s\n\tComponents - %d\n\tDimension - %d x %d", m_ID, m_Path.c_str(),
                channels,
                m_Width,
                m_Height);

        stbi_image_free(data);
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_ID);
    }

    void Texture2D::SetData(void *data, std::uint32_t size)
    {
        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void Texture2D::Bind(std::uint32_t texSlot)
    {
        glBindTextureUnit(texSlot, m_ID);
    }
}