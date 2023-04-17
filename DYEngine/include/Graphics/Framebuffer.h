#pragma once

#include "Graphics/Texture.h"

#include <cstdint>
#include <memory>

namespace DYE
{
	using FramebufferID = std::uint32_t;

	struct FramebufferProperties
	{
		std::uint32_t Width = 0;
		std::uint32_t Height = 0;
		std::uint32_t Samples = 1;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		static std::shared_ptr<Framebuffer> Create(const FramebufferProperties& properties);

		Framebuffer() = delete;
		explicit Framebuffer(FramebufferProperties const &properties);
		~Framebuffer();

		void Bind();
		void Unbind();

		void Resize(std::uint32_t width, std::uint32_t height);

		/// Create the actual underlying implementation instance of the framebuffer.
		/// If it's already created, it will be reset and re-created based on the internal properties.
		void CreateOrReset();

		// TODO: maybe later we want to create a color texture 2d handle so others could access the color attachment as if it's
		// 		a texture 2d, OR MAYBE NOT.
		TextureID GetColorAttachmentID() const { return m_ColorAttachmentID; }
		FramebufferProperties const& GetProperties() const { return m_Properties; }

	private:
		FramebufferID m_ID {0};
		TextureID m_ColorAttachmentID;
		TextureID m_DepthStencilAttachmentID;

		FramebufferProperties m_Properties;
	};
}