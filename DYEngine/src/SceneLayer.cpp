#include "Base.h"
#include "Scene/SceneLayer.h"
#include "Time.h"
#include "WindowBase.h"
#include "Logger.h"
#include "Scene/Entity.h"
#include "Scene/ComponentBase.h"

#include <imgui.h>

namespace DYE
{
    SceneLayer::SceneLayer(WindowBase *pWindow) : m_pWindow(pWindow)
    {

    }

    SceneLayer::~SceneLayer()
    {
        // delete the registered component updater
        for (auto pComponentUpdater : m_pComponentUpdaters)
        {
            delete pComponentUpdater;
        }
    }

    void SceneLayer::OnEvent(const std::shared_ptr<Event> &pEvent)
    {
        /*
        EventDispatcher dispatcher(*pEvent);

        dispatcher.Dispatch<KeyDownEvent>(DYE_BIND_EVENT_FUNCTION(handleOnKeyDown));
        dispatcher.Dispatch<KeyUpEvent>(DYE_BIND_EVENT_FUNCTION(handleOnKeyUp));
        */
    }

    void SceneLayer::OnUpdate()
    {
    }

    void SceneLayer::OnFixedUpdate()
    {
    }

    void SceneLayer::OnImGui()
    {
        static int counter = 0;
        // get the window size as a base for calculating widgets geometry
        int sdl_width = 0, sdl_height = 0, controls_width = 0;
        SDL_GetWindowSize(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>(), &sdl_width, &sdl_height);
        controls_width = sdl_width;
        // make controls widget width to be 1/3 of the main window width
        if ((controls_width /= 3) < 300) { controls_width = 300; }

        // position the controls widget in the top-right corner with some margin
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
        // here we set the calculated width and also make the height to be
        // be the height of the main window also with some margin
        ImGui::SetNextWindowSize(
                ImVec2(static_cast<float>(controls_width), static_cast<float>(sdl_height - 20)),
                ImGuiCond_Always
        );

        // create a window and append into it
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize);
        ImGui::Dummy(ImVec2(0.0f, 1.0f));
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
        ImGui::Text("%s", SDL_GetPlatform());
        ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
        ImGui::Text("RAM: %.2f GB", (float) SDL_GetSystemRAM() / 1024.0f);

        // buttons and most other widgets return true when clicked/edited/activated
        if (ImGui::Button("Counter button"))
        {
            SDL_Log("Counter Button Clicked");
            counter++;
        }

        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("expected FPS: [%d]", TIME.FixedFramePerSecond());
        ImGui::Text("DeltaTime: [%f]", TIME.DeltaTime());
        ImGui::Text("FixedDeltaTime: [%f]", TIME.FixedDeltaTime());

        if (ImGui::Button("Test Message Box Button"))
        {
            DYE_MSG_BOX(SDL_MESSAGEBOX_WARNING, "HAHA", "Test");
        }

        ImGui::End();
    }

    std::weak_ptr<Entity> SceneLayer::CreateEntity(const std::string& name)
    {
        auto newEntity = std::make_shared<Entity>(m_EntityIDCounter, name);
        m_Entities.push_back(newEntity);
        m_EntityIDCounter++;
        return newEntity;
    }

    Entity *SceneLayer::GetEntity(int id)
    {
        for (const auto& ent : m_Entities)
        {
            if (ent->GetID() == id)
            {
                return ent.get();
            }
        }
        return nullptr;
    }

    template<class T>
    ComponentUpdaterBase *SceneLayer::CreateAndRegisterComponentUpdater()
    {
        using UpdaterType = ComponentUpdater<T>;
        UpdaterType* newUpdater = new UpdaterType(m_pComponentUpdaters.size());

        m_pComponentUpdaters.push_back(newUpdater);
        return newUpdater;
    }

    template<class T>
    ComponentUpdaterBase *SceneLayer::GetComponentUpdaterOfType()
    {
        // delete the registered component updater
        for (auto pComponentUpdater : m_pComponentUpdaters)
        {
            if (pComponentUpdater)
        }
        return nullptr;
    }
}