#include "Graphics/WindowManager.h"

#include "ImGui/ImGuiUtil.h"
#include "Util/Macro.h"

#include <algorithm>

namespace DYE
{
    std::vector<std::pair<WindowID, std::unique_ptr<WindowBase>>> WindowManager::s_Windows = {};
    std::optional<WindowID> WindowManager::s_MainWindowID = {};

    WindowBase *WindowManager::CreateWindow(WindowProperties const &windowProperties)
    {
        auto window = WindowBase::Create(windowProperties);
        auto cachePtr = window.get();                        // We cache the raw pointer because unique_ptr would be moved into vector later.
        WindowID const id = window->GetWindowID();
        s_Windows.emplace_back(id, std::move(window));

        if (!s_MainWindowID.has_value())
        {
            // If there is no main window yet, set the newly created window as main.
            s_MainWindowID = id;
        }

        return cachePtr;
    }

    bool WindowManager::CloseWindow(WindowID id)
    {
        auto numberOfErasedWindows = erase_if(s_Windows,
                                              [id](std::pair<WindowID, std::unique_ptr<WindowBase>> &windowPair)
                                              {
                                                  return windowPair.first == id;
                                              });

        return numberOfErasedWindows >= 1;
    }

    void WindowManager::SetMainWindow(WindowID id)
    {
        s_MainWindowID = id;
    }

    WindowBase *WindowManager::TryGetWindowFromID(WindowID id)
    {
        for (auto const &windowPair: s_Windows)
        {
            if (windowPair.first != id)
            {
                continue;
            }

            return windowPair.second.get();
        }

        return nullptr;

    }

    WindowBase *WindowManager::TryGetWindowAt(std::uint32_t index)
    {
        if (index >= s_Windows.size())
        {
            // The index is out of bounds.
            return nullptr;
        }

        return s_Windows[index].second.get();
    }

    std::optional<std::uint32_t> WindowManager::TryGetWindowIndexFromID(WindowID id)
    {
        for (std::uint32_t index = 0; index < s_Windows.size(); index++)
        {
            if (s_Windows[index].first == id)
            {
                return index;
            }
        }

        return {};
    }

    std::optional<WindowID> WindowManager::TryGetMainWindowID()
    {
        return s_MainWindowID;
    }

    WindowBase *WindowManager::GetMainWindow()
    {
        if (!s_MainWindowID.has_value())
        {
            DYE_ASSERT_RELEASE(false && "There is no main window!");
            return nullptr;
        }

        return s_Windows[MainWindowIndex].second.get();
    }

    WindowBase *WindowManager::TryGetMouseFocusedWindow()
    {
        WindowID const id = WindowBase::GetMouseFocusedWindowID();
        return TryGetWindowFromID(id);
    }

    std::optional<WindowID> WindowManager::TryGetMouseFocusedWindowID()
    {
        WindowID windowID = WindowBase::GetMouseFocusedWindowID();
        if (windowID == 0)
        {
            return {};
        }

        return windowID;
    }

    bool WindowManager::HasWindowWithID(WindowID id)
    {
        return std::any_of(s_Windows.begin(), s_Windows.end(), [id](auto const &windowPair) { return windowPair.first == id; });
    }

    bool WindowManager::IsMainWindow(const WindowBase &window)
    {
        if (!s_MainWindowID.has_value())
        {
            return false;
        }

        return window.GetWindowID() == s_MainWindowID.value();
    }

    void WindowManager::UpdateWindows()
    {
        for (auto const &windowPair: s_Windows)
        {
            windowPair.second->OnUpdate();
        }
    }

    void WindowManager::DrawWindowManagerImGui(bool *pIsOpen)
    {
        // Set a default size for the window in case it has never been opened before.
        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Window Manager", pIsOpen))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginTabBar("##WindowManagerTabs", ImGuiTabBarFlags_Reorderable))
        {
            if (ImGui::BeginTabItem("Registered Windows"))
            {
                drawRegisteredWindowsInspectorImGui();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Other"))
            {
                // Do nothing now.
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void WindowManager::drawRegisteredWindowsInspectorImGui()
    {
        // Left - registered windows list.
        static int selectedWindowIndex = 0;

        if (selectedWindowIndex >= s_Windows.size())
        {
            // If the selected index is out-of-bounds, select the last item.
            selectedWindowIndex = s_Windows.size() - 1;
        }

        {
            const float windowWidth = 175;
            ImGui::BeginChild("Registered Windows List", ImVec2(windowWidth, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
            for (int i = 0; i < s_Windows.size(); ++i)
            {
                auto &windowPair = s_Windows[i];
                char label[128];
                sprintf(label, "%03d: %s", i, windowPair.second->GetTitle().c_str());
                label[127] = 0;    // Set the last character to 0 to avoid longer names overflowing the buffer.
                if (ImGui::Selectable(label, selectedWindowIndex == i))
                {
                    selectedWindowIndex = i;
                }
            }
            ImGui::EndChild();
        }
        ImGui::SameLine();

        // Right - selected window info.
        {
            auto &selectedWindowPair = s_Windows[selectedWindowIndex];
            WindowBase *pWindow = selectedWindowPair.second.get();

            auto const originalControlLabelWidth = ImGuiUtil::Settings::ControlLabelWidth;
            ImGuiUtil::Settings::ControlLabelWidth = 100;

            ImGui::BeginGroup();
            ImGui::BeginChild("Selected Window Info", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
                              ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 line below us

            if (IsMainWindow(*pWindow))
            {
                ImGui::Text("(Main Window) %s", pWindow->GetTitle().c_str());
            }
            else
            {
                ImGui::Text("%s", pWindow->GetTitle().c_str());
            }

            ImGui::Separator();
            ImGuiUtil::DrawReadOnlyTextWithLabel("WindowID", std::to_string(selectedWindowPair.first));
            ImGuiUtil::DrawReadOnlyTextWithLabel("Title", pWindow->GetTitle());

            switch (pWindow->GetFullScreenMode())
            {
                case FullScreenMode::Window:
                    ImGuiUtil::DrawReadOnlyTextWithLabel("Mode", "Window");
                    break;
                case FullScreenMode::FullScreen:
                    ImGuiUtil::DrawReadOnlyTextWithLabel("Mode", "FullScreen");
                    break;
                case FullScreenMode::FullScreenWindow:
                    ImGuiUtil::DrawReadOnlyTextWithLabel("Mode", "FullScreenWithDesktopResolution");
                    break;
            }

            auto const position = pWindow->GetPosition();
            ImGuiUtil::DrawReadOnlyTextWithLabel("Position", "(" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
            auto const size = pWindow->GetSize();
            ImGuiUtil::DrawReadOnlyTextWithLabel("Size", "(" + std::to_string(size.x) + ", " + std::to_string(size.y) + ")");

            ImGui::EndChild();
            ImGui::EndGroup();

            ImGuiUtil::Settings::ControlLabelWidth = originalControlLabelWidth;
        }
    }
}
