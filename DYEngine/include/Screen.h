#pragma once

#include <memory>
#include <optional>

#define SCREEN Screen::GetInstance()

namespace DYE
{
	struct DisplayMode
	{
		std::int32_t Width;
		std::int32_t Height;
		std::int32_t RefreshRate;
	};

	class Screen
	{
	private:
		static std::unique_ptr<Screen> s_Instance;

	public:
		static void InitSingleton();
		static Screen& GetInstance();

		std::uint32_t GetNumberOfDisplays() const;
		std::optional<DisplayMode> TryGetDisplayMode(int displayIndex) const;
	};
}
