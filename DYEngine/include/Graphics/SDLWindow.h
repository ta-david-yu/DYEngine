#pragma once

#include "WindowBase.h"
#include "ContextBase.h"

class SDL_Window;

namespace DYE
{
    class SDLWindow final : public WindowBase
    {
    public:
        explicit SDLWindow(const WindowProperty &windowProperty);

        ~SDLWindow() override;

        void OnUpdate() override;

        uint32_t GetWidth() const override;
        uint32_t GetHeight() const override;
		glm::vec<2, std::int32_t> GetPosition() const override;
		FullScreenMode GetFullScreenMode() const override;
		int GetDisplayIndex() const override;

		void *GetNativeWindowPtr() const override { return m_pNativeWindow; }
		std::uint32_t GetWindowID() const override;

		bool SetFullScreenMode(FullScreenMode mode) override;
		bool SetBorderedIfWindowed(bool isBordered) override;
		void SetSize(std::uint32_t width, std::uint32_t height) override;
		void SetPosition(std::int32_t x, std::int32_t y) override;
		void CenterWindow() override;

    private:
        /// The pointer to the native window object
        SDL_Window *m_pNativeWindow;
    };
}