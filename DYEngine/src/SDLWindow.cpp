#include "SDLWindow.h"

#include "Base.h"

#include <SDL.h>

namespace DYE
{
    SDLWindow::SDLWindow(const WindowProperty &windowProperty)
    {
        auto windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        // create SDL window
        m_pNativeWindow = SDL_CreateWindow(
                windowProperty.Title.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                windowProperty.Width,
                windowProperty.Height,
                windowFlags
        );

		std::uint32_t fullScreenFlag = 0;
		switch (windowProperty.FullScreeMode)
		{
			case FullScreenMode::Window:
				fullScreenFlag = 0;
				break;
			case FullScreenMode::FullScreen:
				fullScreenFlag = SDL_WINDOW_FULLSCREEN;
				break;
			case FullScreenMode::BorderlessWindow:
				fullScreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
				break;
		}

		SDL_SetWindowFullscreen(m_pNativeWindow, fullScreenFlag);
        SDL_SetWindowResizable(m_pNativeWindow, (windowProperty.IsUserResizable ? SDL_TRUE : SDL_FALSE));

        // create context
        m_Context = ContextBase::Create(this);
        m_Context->Init();
    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyWindow(m_pNativeWindow);
    }

    void SDLWindow::OnUpdate()
    {
        m_Context->SwapBuffers();
    }

    uint32_t SDLWindow::GetWidth() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_pNativeWindow, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return width;
    }

    uint32_t SDLWindow::GetHeight() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_pNativeWindow, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return height;
    }

	ContextBase &SDLWindow::GetContext() const
	{
		return *m_Context;
	}

	bool SDLWindow::SetFullScreenMode(FullScreenMode mode)
	{
		std::uint32_t fullScreenFlag = 0;
		switch (mode)
		{
			case FullScreenMode::Window:
				fullScreenFlag = 0;
				break;
			case FullScreenMode::FullScreen:
				fullScreenFlag = SDL_WINDOW_FULLSCREEN;
				break;
			case FullScreenMode::BorderlessWindow:
				fullScreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
				break;
		}

		int const result = SDL_SetWindowFullscreen(m_pNativeWindow, fullScreenFlag);
		bool const success = result == 0;
		if (!success)
		{
			DYE_LOG_ERROR("SetFullScreenMode(%d) failed: %s", fullScreenFlag, SDL_GetError());
		}

		return success;
	}

	void SDLWindow::SetWindowSize(uint32_t width, uint32_t height)
	{
		SDL_SetWindowSize(m_pNativeWindow, width, height);
		DYE_LOG_INFO("WindowSize: %d, %d", width, height);
	}

	std::uint32_t SDLWindow::GetWindowID() const
	{
		return SDL_GetWindowID(m_pNativeWindow);
	}
}