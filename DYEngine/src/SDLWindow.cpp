#include "Graphics/SDLWindow.h"

#include "Util/Macro.h"

#include <SDL.h>

namespace DYE
{
    SDLWindow::SDLWindow(const WindowProperties &windowProperties)
    {
        auto windowFlags = static_cast<SDL_WindowFlags>(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

        // create SDL window
        m_pNativeWindow = SDL_CreateWindow(
			windowProperties.Title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowProperties.Width,
			windowProperties.Height,
			windowFlags
        );

		std::uint32_t fullScreenFlag = 0;
		switch (windowProperties.FullScreeMode)
		{
			case FullScreenMode::Window:
				fullScreenFlag = 0;
				break;
			case FullScreenMode::FullScreen:
				fullScreenFlag = SDL_WINDOW_FULLSCREEN;
				break;
			case FullScreenMode::FullScreenWithDesktopResolution:
				fullScreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
				break;
		}

		SDL_SetWindowFullscreen(m_pNativeWindow, fullScreenFlag);
        SDL_SetWindowResizable(m_pNativeWindow, (windowProperties.IsUserResizable ? SDL_TRUE : SDL_FALSE));
    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyWindow(m_pNativeWindow);
    }

    void SDLWindow::OnUpdate()
    {
		// Do nothing.
    }

	std::string SDLWindow::GetTitle() const
	{
		return SDL_GetWindowTitle(m_pNativeWindow);
	}

    uint32_t SDLWindow::GetWidth() const
    {
        uint32_t width, height;
        SDL_GetWindowSize(m_pNativeWindow, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
        return width;
    }

	glm::vec<2, std::uint32_t> SDLWindow::GetSize() const
	{
		uint32_t width, height;
		SDL_GetWindowSize(m_pNativeWindow, reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height));
		return {width, height};
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

	FullScreenMode SDLWindow::GetFullScreenMode() const
	{
		Uint32 flags = SDL_GetWindowFlags(m_pNativeWindow);
		if (flags & SDL_WINDOW_FULLSCREEN)
		{
			return FullScreenMode::FullScreen;
		}
		else if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
		{
			return FullScreenMode::FullScreenWithDesktopResolution;
		}

		return FullScreenMode::Window;
	}

	int SDLWindow::GetDisplayIndex() const
	{
		return SDL_GetWindowDisplayIndex(m_pNativeWindow);
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
			case FullScreenMode::FullScreenWithDesktopResolution:
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

	bool SDLWindow::SetBorderedIfWindowed(bool isBordered)
	{
		if (GetFullScreenMode() != FullScreenMode::Window)
		{
			return false;
		}
		SDL_SetWindowBordered(m_pNativeWindow, isBordered? SDL_TRUE : SDL_FALSE);
		return true;
	}

	void SDLWindow::Minimize()
	{
		SDL_MinimizeWindow(m_pNativeWindow);
	}

	void SDLWindow::Restore()
	{
		SDL_RestoreWindow(m_pNativeWindow);
	}

	void SDLWindow::Raise()
	{
		SDL_RaiseWindow(m_pNativeWindow);
	}

	void SDLWindow::SetTitle(const std::string &name)
	{
		SDL_SetWindowTitle(m_pNativeWindow, name.c_str());
	}

	void SDLWindow::SetSize(std::uint32_t width, std::uint32_t height)
	{
		SDL_SetWindowSize(m_pNativeWindow, width, height);
	}

	void SDLWindow::SetPosition(std::int32_t x, std::int32_t y)
	{
		SDL_SetWindowPosition(m_pNativeWindow, x, y);
	}

	void SDLWindow::CenterWindow()
	{
		SDL_SetWindowPosition(m_pNativeWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}