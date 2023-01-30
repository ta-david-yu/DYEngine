#pragma once

#include "Graphics/WindowBase.h"
#include "Graphics/WindowManager.h"
#include "Event/EventSystemBase.h"
#include "Event/ApplicationEvent.h"
#include "Util/Time.h"
#include "LayerStack.h"
#include "LayerBase.h"
#include "ImGui/ImGuiLayer.h"

#include <memory>
#include <vector>

namespace DYE
{
	struct ApplicationLayerOperation
	{
		enum class OperationType
		{
			PushLayer,
			PopLayer,
			PushOverlay,
			PopOverlay
		};

		OperationType Type;
		std::shared_ptr<LayerBase> Layer;
	};

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
		void Shutdown();

		void PushLayer(std::shared_ptr<LayerBase> &layer);
		void PopLayer(std::shared_ptr<LayerBase> &layer);
		void PushOverlay(std::shared_ptr<LayerBase> &overlay);
		void PopOverlay(std::shared_ptr<LayerBase> &overlay);

    protected:
        void pushLayerImmediate(const std::shared_ptr<LayerBase> &layer);
		void popLayerImmediate(std::shared_ptr<LayerBase> layer);
        void pushOverlayImmediate(const std::shared_ptr<LayerBase> &overlay);
		void popOverlayImmediate(std::shared_ptr<LayerBase> overlay);

        /// Called after ForEach.layer.OnUpdate(), before ForEach.layer.OnRender()
        virtual void onPreRenderLayers() { }
        /// Called after ForEach.layer.OnRender(), before BeginImGui()
        virtual void onPostRenderLayers() { }

    private:
		void handleOnApplicationQuit(ApplicationQuitEvent const& event);
        void handleOnWindowClose(const WindowCloseEvent& event);

    protected:
        /// The main event system that poll events from the framework (SDL/GLFW for instance)
        std::unique_ptr<EventSystemBase> m_EventSystem;
        std::shared_ptr<ImGuiLayer> m_ImGuiLayer;

    private:
        LayerStack m_LayerStack;
		std::vector<ApplicationLayerOperation> m_LayerOperations;

        /// A Flag to show if the game is still running, when WindowCloseEvent is fired, it's set to false
        bool m_IsRunning = false;
    };

    // To be defined in CLIENT code
    Application* CreateApplication();
}
