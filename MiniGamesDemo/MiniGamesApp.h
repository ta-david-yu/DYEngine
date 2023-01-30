#pragma once

#include "Core/Application.h"

namespace DYE
{
	class LayerBase;

	class MiniGamesApp final : public Application
	{
	public:
		explicit MiniGamesApp(const std::string &windowName, int fixedFramePerSecond = 60);
		MiniGamesApp() = delete;
		MiniGamesApp(const MiniGamesApp &) = delete;

		~MiniGamesApp() final = default;

		void LoadMainMenuLayer();
		void LoadPongLayer();
		void LoadLandBallLayer();

	private:
		std::shared_ptr<LayerBase> m_CurrentMainLayer;
	};
}