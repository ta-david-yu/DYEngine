#include "MiniGamesDemo/MiniGamesApp.h"

#include "AppEntryPoint.h"

#include "Layers/MainMenuLayer.h"
#include "Layers/LandTheBallLayer.h"
#include "Layers/PongLayer.h"

#include "Input/InputManager.h"

namespace DYE
{
	MiniGamesApp::MiniGamesApp(const std::string &windowName, int fixedFramePerSecond)
		: Application(windowName, fixedFramePerSecond)
	{
		m_CurrentMainLayer = std::make_shared<MainMenuLayer>(*this);
		pushLayerImmediate(m_CurrentMainLayer);

		INPUT.EnableGamepadInputEventInBackground();
	}

	void MiniGamesApp::LoadMainMenuLayer()
	{
		PopLayer(m_CurrentMainLayer);
		m_CurrentMainLayer = std::make_shared<MainMenuLayer>(*this);
		PushLayer(m_CurrentMainLayer);
	}

	void MiniGamesApp::LoadPongLayer()
	{
		PopLayer(m_CurrentMainLayer);
		m_CurrentMainLayer = std::make_shared<PongLayer>(*this);
		PushLayer(m_CurrentMainLayer);
	}

	void MiniGamesApp::LoadLandBallLayer()
	{
		PopLayer(m_CurrentMainLayer);
		m_CurrentMainLayer = std::make_shared<LandTheBallLayer>(*this);
		PushLayer(m_CurrentMainLayer);
	}
}

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new MiniGamesApp {"DYE Tech Demo", 60};
}
