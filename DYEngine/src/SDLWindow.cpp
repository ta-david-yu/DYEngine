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
    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyWindow(m_pNativeWindow);
    }

    void SDLWindow::OnUpdate()
    {
		// Do nothing.
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

	glm::vec<2, std::int32_t> SDLWindow::GetPosition() const
	{
		int x, y;
		SDL_GetWindowPosition(m_pNativeWindow, &x, &y);
		return glm::vec<2, std::int32_t>(x, y);
	}

	std::uint32_t SDLWindow::GetWindowID() const
	{
		return SDL_GetWindowID(m_pNativeWindow);
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

	void SDLWindow::SetWindowSize(std::uint32_t width, std::uint32_t height)
	{
		SDL_SetWindowSize(m_pNativeWindow, width, height);
	}

	void SDLWindow::SetWindowPosition(std::int32_t x, std::int32_t y)
	{
		SDL_SetWindowPosition(m_pNativeWindow, x, y);
	}

	void SDLWindow::CenterWindow()
	{
		SDL_SetWindowPosition(m_pNativeWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}