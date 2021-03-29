#include "Graphics/Texture.h"
#include "Graphics/OpenGL.h"
#include "Base.h"

#include <SDL_image.h>

namespace DYE
{
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

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
        glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    /// Temporary flip SDL_Surface code
    static void flip_surface(SDL_Surface* surface)
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

    Texture2D::Texture2D(const std::string &path) : m_Path(path)
    {
        SDL_Surface* surface = IMG_Load(path.c_str());

        if (surface != nullptr)
        {
            flip_surface(surface);
            m_Width = surface->w;
            m_Height = surface->h;
        }
        else
        {
            DYE_LOG("Failed to load texture %s!", path.c_str());
            DYE_ASSERT(false);
        }

        if (surface->format->BytesPerPixel == 4)
        {
            m_InternalFormat = GL_RGBA8;
            m_DataFormat = GL_RGBA;
        }
        else if (surface->format->BytesPerPixel == 3)
        {
            m_InternalFormat = GL_RGB8;
            m_DataFormat = GL_RGB;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
        glTextureStorage2D(m_ID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_ID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, surface->pixels);
        SDL_FreeSurface(surface);
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