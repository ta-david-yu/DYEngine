#include "DYETechDemoApp.h"

#include "AppEntryPoint.h"

#include "Layers/MainMenuLayer.h"
#include "Layers/LandTheBallLayer.h"
#include "Layers/PongLayer.h"

#include "Input/InputManager.h"

namespace DYE
{
	DYETechDemoApp::DYETechDemoApp(const std::string &windowName, int fixedFramePerSecond)
		: Application(windowName, fixedFramePerSecond)
	{
		m_CurrentMainLayer = std::make_shared<MainMenuLayer>(*this);
		pushLayerImmediate(m_CurrentMainLayer);

		INPUT.EnableGamepadInputEventInBackground();
	}

	void DYETechDemoApp::LoadMainMenuLayer()
	{
		PopLayer(m_CurrentMainLayer);
		m_CurrentMainLayer = std::make_shared<MainMenuLayer>(*this);
		PushLayer(m_CurrentMainLayer);
	}

	void DYETechDemoApp::LoadPongLayer()
	{
		PopLayer(m_CurrentMainLayer);
		m_CurrentMainLayer = std::make_shared<PongLayer>(*this);
		PushLayer(m_CurrentMainLayer);
	}

	void DYETechDemoApp::LoadLandBallLayer()
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
    return new DYETechDemoApp {"DYE Tech Demo", 60};
}
