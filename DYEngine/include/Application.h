#pragma once

#include "WindowBase.h"
#include "Events/EventSystemBase.h"
#include "Time.h"

#include <memory>

namespace DYE
{
    class Application
    {
    public:
        Application() = delete;

        Application(const Application &) = delete;

        /// Application Constructor
        /// \param windowName: The name shown at the top of the app window
        explicit Application(const std::string &windowName = "DYE App", int fixedFramePerSecond = 60);

        /// App Entrypoint
        void Run();

    private:

        /// The main rendering window
        std::unique_ptr<WindowBase> m_Window;

        /// The main event system that poll events from the framework (SDL/GLFW for instance)
        std::unique_ptr<EventSystemBase> m_EventSystem;

        /// Time for counting delta time per frame
        Time m_Time;

        /// A Flag to show if the game is still running, when WindowCloseEvent is fired, it's set to false
        bool m_IsRunning = false;

        /// The EventHandler that handles Events polled by m_EventSystem
        /// \param pEvent the received event pointer
        /// \return true if the event can be handled
        bool handleOnEvent(const std::shared_ptr<Event>& pEvent);
    };

    // To be defined in CLIENT code
    Application* CreateApplication();
}
