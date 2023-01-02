#include "ImGuiLayer.h"

#include "WindowBase.h"
#include "ContextBase.h"
#include "Graphics/OpenGL.h"
#include "WindowManager.h"

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
		io.ConfigWindowsMoveFromTitleBarOnly = true;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        auto glsl_version = "#version 130";
		// TODO: Maybe move these code to somewhere else to support different platforms libraries
		// 	Or just use preprocessor to detect different platforms or GPU API :P
        ImGui_ImplSDL2_InitForOpenGL(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>(), m_pWindow->GetContext()->GetNativeContextPtr());
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
		// It's important to make the attaching window current first,
		// So ImGUI viewports/frame are properly rendered on the attaching window.
		m_pWindow->MakeCurrent();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::EndImGui()
    {
        // rendering
        ImGui::Render();

		ImGuiIO& io = ImGui::GetIO();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			// Make the window context current again
			// in case there are imgui viewports which are separate windows from the main window.
			m_pWindow->MakeCurrent();
		}
    }
}