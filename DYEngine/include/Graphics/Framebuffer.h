#pragma once

#include "Graphics/Texture.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace DYE
{
	using FramebufferID = std::uint32_t;

	enum class FramebufferTextureFormat
	{
		None = 0,

		// TODO: add more framebuffer texture format
		// Color
		RGBA8,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureProperties
	{
		FramebufferTextureProperties() = default;
		FramebufferTextureProperties(FramebufferTextureFormat format) : TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filter/wrap mode, could possibly use Texture::Wrap/Filter Mode
	};

	struct FramebufferAttachmentProperties
	{
		FramebufferAttachmentProperties() = default;
		FramebufferAttachmentProperties(std::initializer_list<FramebufferTextureProperties> attachments) : TexturePropertiesList(attachments) {}

		std::vector<FramebufferTextureProperties> TexturePropertiesList;
	};

	struct FramebufferProperties
	{
		std::uint32_t Width = 0;
		std::uint32_t Height = 0;
		std::uint32_t Samples = 1;
		bool SwapChainTarget = false;
		FramebufferAttachmentProperties Attachments;
	};

	class Framebuffer
	{
	public:
		static std::shared_ptr<Framebuffer> Create(const FramebufferProperties& properties);

		Framebuffer() = delete;
		explicit Framebuffer(FramebufferProperties properties);
		~Framebuffer();

		void Bind();
		void Unbind();

		void Resize(std::uint32_t width, std::uint32_t height);

		/// Create the actual underlying implementation instance of the framebuffer.
		/// If it's already created, it will be reset and re-created based on the internal properties.
		void CreateOrReset();

		// TODO: maybe later we want to create a color texture 2d handle so others could access the color attachment as if it's
		// 		a texture 2d, OR MAYBE NOT.
		TextureID GetColorAttachmentID(std::size_t index) const;
		FramebufferProperties const& GetProperties() const { return m_Properties; }

		void SetDebugLabel(std::string const &name);

	private:
		// By default, 0 means texture is not created on the GPU.
		FramebufferID m_ID {0};
		FramebufferProperties m_Properties;

		std::vector<FramebufferTextureProperties> m_ColorAttachmentPropertiesList;
		FramebufferTextureProperties m_DepthAttachmentProperties = FramebufferTextureFormat::None;

		std::vector<TextureID> m_ColorAttachmentIDs;
		TextureID  m_DepthStencilAttachmentID;

		std::string m_DebugName;
	};
}