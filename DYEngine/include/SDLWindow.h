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
		ContextBase & GetContext() const override;

		bool SetFullScreenMode(FullScreenMode mode) override;
		void SetWindowSize(uint32_t width, uint32_t height) override;

        void *GetNativeWindowPtr() const override { return m_pNativeWindow; }
		std::uint32_t GetWindowID() const override;

    private:
        /// The pointer to the native window object
        SDL_Window *m_pNativeWindow;

        /// The underlying GraphicsContext
        std::unique_ptr<ContextBase> m_Context;
    };
}