#include "ImGuiLayer.h"

#include "WindowBase.h"

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

namespace DYE
{
    ImGuiLayer::ImGuiLayer(WindowBase* pWindow) :
        LayerBase("ImGuiLayer"),
        m_pWindow(pWindow)
    {
    }

    void ImGuiLayer::OnAttach()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
		// TODO: we want to move these somewhere the users can modify them.
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		//ImGUiConf

        ImGui::StyleColorsDark();

        auto glsl_version = "#version 130";
        ImGui_ImplSDL2_InitForOpenGL(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>(), SDL_GL_GetCurrentContext());
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void ImGuiLayer::OnDetach()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnEvent(Event& event)
    {
        if (m_BlockEvents)
        {
            // Use up an event if ImGui wants it
            auto& io = ImGui::GetIO();
            event.IsUsed |= event.IsInCategory(EventCategory::Mouse) && io.WantCaptureMouse;
            event.IsUsed |= event.IsInCategory(EventCategory::Keyboard) && io.WantCaptureKeyboard;
            // TODO: use EventCategory::TextInput
        }
    }

    void ImGuiLayer::BeginImGui()
    {
        // start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>());
        ImGui::NewFrame();
    }

    void ImGuiLayer::EndImGui()
    {
        // rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();

		// TODO:
    }
}