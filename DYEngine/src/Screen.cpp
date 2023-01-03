#include "Screen.h"

#include "Base.h"

namespace DYE
{
	std::unique_ptr<Screen> Screen::s_Instance = {};

	void Screen::InitSingleton()
	{
		// TODO: Add more platform specific Screen child class: SDLScreen etc
		s_Instance = std::make_unique<Screen>();
	}

	Screen& Screen::GetInstance()
	{
		DYE_ASSERT_RELEASE(
			s_Instance != nullptr &&
			"You might have forgot to call Screen::InitSingleton beforehand?");
		return *s_Instance;
	}

	std::uint32_t Screen::GetNumberOfDisplays() const
	{
		return SDL_GetNumVideoDisplays();
	}

	std::optional<DisplayMode> Screen::GetDisplayMode(int displayIndex) const
	{
		SDL_DisplayMode nativeDisplayMode;
		bool const success = SDL_GetCurrentDisplayMode(displayIndex, &nativeDisplayMode) == 0;

		if (!success)
		{
			DYE_LOG_ERROR("GetDisplayMode(%d) failed: %s", displayIndex, SDL_GetError());
			return {};
		}

		return DisplayMode { .Width = nativeDisplayMode.w, .Height = nativeDisplayMode.h, .RefreshRate = nativeDisplayMode.refresh_rate };
	}


}