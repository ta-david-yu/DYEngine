#pragma once

#include "Core/Application.h"

namespace DYE
{
	class LayerBase;

	class DYETechDemoApp final : public Application
	{
	public:
		explicit DYETechDemoApp(const std::string &windowName, int fixedFramePerSecond = 60);
		DYETechDemoApp() = delete;
		DYETechDemoApp(const DYETechDemoApp &) = delete;

		~DYETechDemoApp() final = default;

		void LoadMainMenuLayer();
		void LoadPongLayer();
		void LoadLandBallLayer();

	private:
		std::shared_ptr<LayerBase> m_CurrentMainLayer;
	};
}