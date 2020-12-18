#pragma once

#include "WindowBase.h"
#include "Events/EventSystemBase.h"
#include "Events/ApplicationEvent.h"
#include "Time.h"
#include "LayerStack.h"
#include "LayerBase.h"
#include "ImGuiLayer.h"

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

        virtual ~Application();

        /// App Entrypoint
        void Run();

    protected:
        void pushLayer(std::shared_ptr<LayerBase> layer);
        void pushOverlay(std::shared_ptr<LayerBase> overlay);

    private:
        /// The EventHandler that handles Events polled by m_EventSystem
        /// \param pEvent the received event pointer
        /// \return true if the event can be handled
        bool handleOnEvent(const std::shared_ptr<Event>& pEvent);

        /// The EventHandler that handles OnWindowClose Event, set IsRunning to false
        /// \param event WindowCloseEvent
        /// \return true if the event can be handled
        bool handleOnWindowClose(const WindowCloseEvent& event);

    protected:
        /// The main rendering window
        std::unique_ptr<WindowBase> m_Window;

        /// The main event system that poll events from the framework (SDL/GLFW for instance)
        std::unique_ptr<EventSystemBase> m_EventSystem;

        std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

        /// Time for counting delta time per frame
        Time m_Time;

    private:
        LayerStack m_LayerStack;

        /// A Flag to show if the game is still running, when WindowCloseEvent is fired, it's set to false
        bool m_IsRunning = false;
    };

    // To be defined in CLIENT code
    Application* CreateApplication();
}
