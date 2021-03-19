#include "Base.h"
#include "Scene/SceneLayer.h"
#include "Time.h"
#include "WindowBase.h"
#include "Logger.h"
#include "Scene/Entity.h"
#include "Util/Type.h"

#include <imgui.h>

namespace DYE
{
    SceneLayer::SceneLayer(WindowBase *pWindow) : m_pWindow(pWindow)
    {

    }

    SceneLayer::~SceneLayer()
    {
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
        for (auto& updater : m_ComponentUpdaters)
        {
            updater->UpdateComponents();
        }
    }

    void SceneLayer::OnFixedUpdate()
    {
        for (auto& updater : m_ComponentUpdaters)
        {
            updater->FixedUpdateComponents();
        }
    }

    void SceneLayer::OnImGui()
    {
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

        // create a debugger window for the scene layer
        ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        {
            if (ImGui::CollapsingHeader("Platform"))
            {
                ImGui::Dummy(ImVec2(0.0f, 1.0f));
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
                if (ImGui::Button("Test Message Box Button"))
                {
                    DYE_MSG_BOX(SDL_MESSAGEBOX_WARNING, "HAHA", "Test");
                }
            }

            if (ImGui::CollapsingHeader("Entity"))
            {
                float entityViewHeight = 500;
                static int selectedEntityIndex = 0;
                if (selectedEntityIndex >= m_Entities.size())
                {
                    selectedEntityIndex = 0;
                }

                // Left
                {
                    ImGui::BeginChild("left pane", ImVec2(200, entityViewHeight), true);
                    for (int i = 0; i < m_Entities.size(); i++)
                    {
                        auto& entity = m_Entities[i];

                        char label[128];
                        sprintf(label, "%s##%d", entity->GetName().c_str(), i);
                        if (ImGui::Selectable(label, selectedEntityIndex == i))
                        {
                            selectedEntityIndex = i;
                        }
                    }
                    ImGui::EndChild();
                }
                ImGui::SameLine();

                // Right
                {
                    auto& entity = m_Entities[selectedEntityIndex];

                    ImGui::BeginGroup();
                    ImGui::BeginChild("entity view", ImVec2(0, entityViewHeight)); // Leave room for 1 line below us
                    ImGui::Text("[ID: %d] %s", entity->GetID(), entity->GetName().c_str());
                    ImGui::Separator();

                    char componentViewLabel[128];
                    sprintf(componentViewLabel, "Components##%d", entity->GetName().c_str(), selectedEntityIndex);
                        for (auto& compPair : entity->m_Components)
                        {
                            auto comp = compPair.second.lock();
                            ImGui::Checkbox(demangleCTypeName(compPair.first.name()).c_str(), &comp->m_IsEnabled);
                            //ImGui::TextWrapped("%s",  demangleCTypeName(compPair.first.name()).c_str());
                        }

                    /*
                    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                    {
                        if (ImGui::BeginTabItem("Description"))
                        {
                            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Details"))
                        {
                            ImGui::Text("ID: 0123456789");
                            ImGui::EndTabItem();
                        }
                        ImGui::EndTabBar();
                    }*/
                    ImGui::EndChild();
                    ImGui::EndGroup();
                }
            }

            if (ImGui::CollapsingHeader("Updater"))
            {

            }
        }
        ImGui::End();


        ImGui::ShowDemoWindow();
    }

    std::weak_ptr<Entity> SceneLayer::CreateEntity(const std::string& name)
    {
        m_Entities.push_back(std::make_shared<Entity>(m_EntityIDCounter, name));
        m_EntityIDCounter++;
        return m_Entities.back();
    }

    void SceneLayer::DestroyEntity(uint32_t entityID)
    {
        /// TODO: clean up entries in the updaters, list
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

    std::weak_ptr<ComponentUpdaterBase> SceneLayer::CreateAndRegisterGenericComponentUpdater(ComponentTypeID typeID)
    {
        auto [isRegistered, registeredIndex] = getUpdaterIndex(typeID);

        if (isRegistered)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "A component updater with the given typeID has already been registered, at pos %d", registeredIndex);
        }

        // Create a generic updater and cast it to updater base
        m_ComponentUpdaters.push_back(
                std::static_pointer_cast<ComponentUpdaterBase>(
                        std::make_shared<GenericComponentUpdater>(typeID))
                );

        return m_ComponentUpdaters.back();
    }

    void SceneLayer::RegisterComponentUpdater(const std::shared_ptr<ComponentUpdaterBase>& updater)
    {
        auto [isRegistered, registeredIndex] = getUpdaterIndex(updater->GetTypeID());

        if (isRegistered)
        {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "A component updater with the given typeID has already been registered, at pos %d", registeredIndex);
        }

        m_ComponentUpdaters.push_back(updater);
    }

    ComponentUpdaterBase *SceneLayer::GetComponentUpdaterOfType(ComponentTypeID typeID)
    {
        // delete the registered component updater
        for (auto& updater : m_ComponentUpdaters)
        {
            if (updater->GetTypeID() == typeID)
            {
                return updater.get();
            }
        }
        return nullptr;
    }
}