#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include <glm/glm.hpp>

namespace DYE
{
    using TextureID = std::uint32_t;
    using TextureFormat = std::uint32_t;

	enum class WrapMode
	{
		// TODO: to add more texture wrap modes
		Repeat,
		Clamp
	};

	enum class FilterMode
	{
		// TODO: to add more texture filter modes
		Nearest,
		Linear
	};

    /// A base interface class for any types of texture resource
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual std::uint32_t GetWidth() const = 0;
        virtual std::uint32_t GetHeight() const = 0;
        virtual TextureID GetID() const = 0;

        virtual void SetData(void* data, std::uint32_t size) = 0;
        virtual void Bind(std::uint32_t texSlot) = 0;

		void SetWrapMode(WrapMode wrapMode);
		void SetFilterMode(FilterMode filterMode);

		void SetDebugLabel(std::string const &name);
    };

    class Texture2D : public Texture
    {
    public:
        /// Create an empty Texture2D with the given dimensions
        /// \param width
        /// \param height
        /// \return
        static std::shared_ptr<Texture2D> Create(std::uint32_t width, std::uint32_t height);

        /// Create a 1x1 Texture2D of the given color
        /// \param color
        /// \return
        static std::shared_ptr<Texture2D> Create(glm::vec4 color);

		static std::shared_ptr<Texture2D> Create(glm::vec4 color, std::uint32_t width, std::uint32_t height);

        /// Create a Texture2D loaded from the given file path
        /// \return
        static std::shared_ptr<Texture2D> Create(const std::filesystem::path& path);

		/// \return a 1x1 white Texture2D
		static std::shared_ptr<Texture2D> GetWhiteTexture();

		/// Any unassigned texture should be default pointing to this.
		/// It's basically the same as Texture2D::GetWhiteTexture().
		static std::shared_ptr<Texture2D> GetDefaultTexture();

        Texture2D() = delete;

        explicit Texture2D(std::uint32_t width, std::uint32_t height);
        explicit Texture2D(const std::filesystem::path& path);
        ~Texture2D() override;

        std::uint32_t GetWidth() const override { return m_Width; }
        std::uint32_t GetHeight() const override { return m_Height; }

		glm::vec3 GetScaleFromTextureDimensions() const { return glm::vec3 { (float) m_Width / (float) PixelsPerUnit, (float) m_Height / (float) PixelsPerUnit, 1 }; }
		/// Get the transform matrix with scale calculated based on the dimensions (width/height) of the texture & PixelsPerUnit.
		/// \return a transform scaling matrix
		glm::mat4 GetScaleMatrixFromTextureDimensions() const;

        TextureID GetID() const override { return m_ID; }
        auto GetPath() const -> std::filesystem::path { return m_Path; }

        void SetData(void *data, std::uint32_t size) override;
        void Bind(std::uint32_t textureUnitSlot) override;

	public:
		std::uint32_t PixelsPerUnit {100};

    private:
		std::filesystem::path m_Path {};
		std::uint32_t m_Width {0}, m_Height {0};
        TextureID m_ID {0};
        TextureFormat m_InternalFormat {};
		TextureFormat m_DataFormat {};
    };
}