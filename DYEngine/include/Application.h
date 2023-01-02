#pragma once

#include "WindowBase.h"
#include "WindowManager.h"
#include "Event/EventSystemBase.h"
#include "Event/ApplicationEvent.h"
#include "Util/Time.h"
#include "LayerStack.h"
#include "LayerBase.h"
#include "ImGuiLayer.h"

#include <memory>

namespace DYE
{
    class Application : public EventHandler
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

        void Handle(Event& event) override;

    protected:
        void pushLayer(std::shared_ptr<LayerBase> layer);
        void pushOverlay(std::shared_ptr<LayerBase> overlay);

        /// Called after ForEach.layer.OnUpdate(), before ForEach.layer.OnRender()
        virtual void onPreRenderLayers() { }
        /// Called after ForEach.layer.OnRender(), before BeginImGui()
        virtual void onPostRenderLayers() { }

    private:
        void handleOnWindowClose(const WindowCloseEvent& event);
        void handleOnWindowSizeChange(const WindowSizeChangeEvent& event);

    protected:
        /// The main event system that poll events from the framework (SDL/GLFW for instance)
        std::unique_ptr<EventSystemBase> m_EventSystem;

        std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

    private:
        LayerStack m_LayerStack;

        /// A Flag to show if the game is still running, when WindowCloseEvent is fired, it's set to false
        bool m_IsRunning = false;
    };

    // To be defined in CLIENT code
    Application* CreateApplication();
}
