#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

namespace DYE
{
    using TextureID = std::uint32_t;
    using TextureFormat = std::uint32_t;

    /// A base class for any types of texture resource
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual std::uint32_t GetWidth() const = 0;
        virtual std::uint32_t GetHeight() const = 0;
        virtual TextureID GetID() const = 0;

        virtual void SetData(void* data, std::uint32_t size) = 0;
        virtual void Bind(std::uint32_t texSlot) = 0;
    };

    class Texture2D : public Texture
    {
    public:
        /// Create an empty texture2D with the given dimensions
        /// \param width
        /// \param height
        /// \return
        static std::shared_ptr<Texture2D> Create(std::uint32_t width, std::uint32_t height);

        /// Create a 1x1 texture2D of the given color
        /// \param color
        /// \return
        static std::shared_ptr<Texture2D> Create(glm::vec4 color);

        /// Create a texture2D loaded from the given file path
        /// \return
        static std::shared_ptr<Texture2D> Create(const std::string& path);

        Texture2D() = delete;

        explicit Texture2D(std::uint32_t width, std::uint32_t height);
        explicit Texture2D(const std::string& path);
        ~Texture2D();

        std::uint32_t GetWidth() const override { return m_Width; }
        std::uint32_t GetHeight() const override { return m_Height; }
        TextureID GetID() const override { return m_ID; }
        std::string GetPath() const { return m_Path; }

        void SetData(void *data, std::uint32_t size) override;
        void Bind(std::uint32_t textureUnitSlot) override;

    private:
        std::string m_Path {"empty"};
        std::uint32_t m_Width {0}, m_Height {0};
        TextureID m_ID {0};
        TextureFormat m_InternalFormat, m_DataFormat;
    };
}