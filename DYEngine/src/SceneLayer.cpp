#include "Scene/SceneLayer.h"

#include "Base.h"
#include "Time.h"
#include "WindowBase.h"
#include "Logger.h"
#include "Scene/Entity.h"
#include "Scene/Transform.h"
#include "Util/Type.h"

#include <algorithm>
#include <imgui.h>

namespace DYE
{
    SceneLayer::SceneLayer(WindowBase *pWindow) : m_pWindow(pWindow)
    {
        SetupDefaultUpdaters();
    }

    SceneLayer::~SceneLayer()
    {
    }

    void SceneLayer::SetupDefaultUpdaters()
    {
        auto transformUpdater = std::make_shared<TransformUpdater>(ComponentTypeID(typeid(Transform)));
        m_TransformUpdater = transformUpdater;
        RegisterComponentUpdater(std::move(transformUpdater));
    }

    void SceneLayer::OnEvent(const std::shared_ptr<Event> &pEvent)
    {
#if DYE_DEBUG
        /// Don't dispatch debug input, show window if F1
        if (!m_SceneDebugWindowIsOpen && pEvent->GetEventType() == EventType::KeyDown)
        {
            auto keyCode = static_cast<KeyDownEvent&>(*pEvent).GetKeyCode();
            /// Toggle scene debug window
            if (keyCode == KeyCode::F1)
            {
                m_SceneDebugWindowIsOpen = true;
            }
        }
#endif
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
#if DYE_DEBUG
        static uint32_t selectedEntityID = 0;
        if (m_SceneDebugWindowIsOpen)
        {
            int debugWindowWidth = m_pWindow->GetWidth();
            int debugWindowMaxHeight = m_pWindow->GetHeight();
            // make controls widget width to be 1/4 of the main window width
            if ((debugWindowWidth /= 4) < 300) { debugWindowWidth = 300; }
            ImGui::SetNextWindowSizeConstraints(ImVec2(static_cast<float>(debugWindowWidth), 500), ImVec2(FLT_MAX, static_cast<float>(debugWindowMaxHeight - 20)));

            // create a debugger window for the scene layer
            if (ImGui::Begin("Scene", &m_SceneDebugWindowIsOpen))
            {
                if (ImGui::CollapsingHeader("Platform"))
                {
                    ImGui::Dummy(ImVec2(0.0f, 1.0f));
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
                    ImGui::Text("Delta Time: %f", TIME.DeltaTime());
                    ImGui::Text("Fixed Delta Time: %f", TIME.FixedDeltaTime());
                    if (ImGui::Button("Test Message Box Button"))
                    {
                        DYE_MSG_BOX(SDL_MESSAGEBOX_WARNING, "HAHA", "Test");
                    }
                }

                if (ImGui::CollapsingHeader("Entity"))
                {
                    float entityViewHeight = 350;
                    float entityHierarchyWidth = 200;
                    if (m_Entities.find(selectedEntityID) == m_Entities.end())
                    {
                        selectedEntityID = m_Entities.begin()->first;
                    }

                    /// Entity Hierarchy: Left
                    {
                        ImGui::BeginChild("entity hierarchy", ImVec2(entityHierarchyWidth, entityViewHeight), true);
                        for (const auto &entPair : m_Entities)
                        {
                            auto entity = entPair.second;
                            char entityLabel[128];
                            sprintf(entityLabel, "%s##%d", entity->GetName().c_str(), entity->GetID());
                            if (ImGui::Selectable(entityLabel, selectedEntityID == entity->GetID()))
                            {
                                selectedEntityID = entity->GetID();
                            }
                        }
                        ImGui::EndChild();
                    }
                    ImGui::SameLine();

                    /// Entity Info: Right
                    {
                        ImGui::BeginGroup();
                        ImGui::BeginChild("entity info", ImVec2(0, entityViewHeight)); // Leave room for 1 line below us

                        if (!m_Entities.empty())
                        {
                            auto &entity = m_Entities.find(selectedEntityID)->second;

                            ImGui::Text("[ID: %d] %s", entity->GetID(), entity->GetName().c_str());
                            ImGui::Separator();

                            if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                            {
                                if (ImGui::BeginTabItem("Components"))
                                {
                                    /// draw each component view
                                    for (auto &compPair : entity->m_Components)
                                    {
                                        const auto &compName = demangleCTypeName(compPair.first.name());
                                        auto comp = compPair.second.lock();

                                        char componentViewLabel[128];
                                        sprintf(componentViewLabel, "%s##", compName.c_str());

                                        if (ImGui::TreeNode(componentViewLabel))
                                        {
                                            if (ImGui::Button("goto"))
                                            {
                                                m_ComponentDebugWindowIsOpen = true;
                                                m_DebugTargetComponent = comp;
                                            }
                                            ImGui::SameLine();
                                            ImGui::Checkbox("IsEnabled", &comp->m_IsEnabled);

                                            ImGui::TreePop();
                                            ImGui::Separator();
                                        }
                                    }
                                    ImGui::EndTabItem();
                                }
                                if (ImGui::BeginTabItem("Util"))
                                {
                                    if (ImGui::Button("Destroy Entity"))
                                    {
                                        ImmediateDestroyEntity(selectedEntityID);
                                    }
                                    ImGui::EndTabItem();
                                }

                                ImGui::EndTabBar();
                            }
                        }
                        else
                        {
                            ImGui::Text("No entity");
                            ImGui::Separator();
                        }

                        ImGui::EndChild();
                        ImGui::EndGroup();
                    }
                }

                if (ImGui::CollapsingHeader("Updater"))
                {
                    float updaterViewHeight = 350;
                    float updaterListViewHeight = 150;
                    float compListViewWidth = 200;

                    static int selectedUpdaterIndex = 0;
                    if (selectedUpdaterIndex >= m_ComponentUpdaters.size())
                    {
                        selectedUpdaterIndex = 0;
                    }

                    ImGui::BeginChild("updater view", ImVec2(0, updaterViewHeight), false);
                    {
                        // Updater List: Upper
                        {
                            ImGui::BeginChild("updater list view", ImVec2(0, updaterListViewHeight), true);
                            for (int i = 0; i < m_ComponentUpdaters.size(); i++)
                            {
                                const auto &updater = m_ComponentUpdaters[i];

                                char updaterLabel[128];
                                sprintf(updaterLabel, "%d: %s [%s]##%d", i,
                                        demangleCTypeName(updater->GetTypeID().name()).c_str(),
                                        getTypeName(*updater).c_str(), i);
                                if (ImGui::Selectable(updaterLabel, selectedUpdaterIndex == i))
                                {
                                    selectedUpdaterIndex = i;
                                }
                            }
                            ImGui::EndChild();
                        }

                        // Updater Info: Lower
                        if (!m_ComponentUpdaters.empty())
                        {
                            const auto &updater = m_ComponentUpdaters[selectedUpdaterIndex];

                            ImGui::BeginChild("updater info view", ImVec2(0, 0), false);

                            static int selectedUpdaterComponentIndex = 0;
                            if (selectedUpdaterComponentIndex >= updater->m_Components.size())
                            {
                                selectedUpdaterComponentIndex = 0;
                            }

                            // Updater Comps: Left
                            {
                                ImGui::BeginChild("updater components list view", ImVec2(compListViewWidth, 0), true);

                                for (int i = 0; i < updater->m_Components.size(); i++)
                                {
                                    const auto &compPair = updater->m_Components[i];

                                    char updaterCompLabel[128];
                                    sprintf(updaterCompLabel, "%s##%d",
                                            compPair.second->GetEntityPtr()->GetName().c_str(), selectedUpdaterIndex);
                                    if (ImGui::Selectable(updaterCompLabel, selectedUpdaterComponentIndex == i))
                                    {
                                        selectedUpdaterComponentIndex = i;
                                    }
                                }
                                ImGui::EndChild();
                            }
                            ImGui::SameLine();

                            // Comp Info: Right
                            {
                                ImGui::BeginGroup();

                                if (!updater->m_Components.empty())
                                {
                                    const auto &compPair = updater->m_Components[selectedUpdaterComponentIndex];
                                    auto compPtr = compPair.second.get();
                                    auto compTypeID = ComponentTypeID(typeid(*compPtr));
                                    auto compName = demangleCTypeName(compTypeID.name());
                                    auto updaterTypeName = demangleCTypeName(updater->GetTypeID().name());
                                    auto entPtr = compPair.second->GetEntityPtr();

                                    char entityLabel[128];
                                    sprintf(entityLabel, "[EntID: %d] %s", entPtr->GetID(), entPtr->GetName().c_str());

                                    ImGui::Text("%s", entityLabel);
                                    if (ImGui::Button("Select this entity in hierarchy"))
                                    {
                                        selectedEntityID = entPtr->GetID();
                                    }
                                    ImGui::Separator();

                                    ImGui::Text("%s", compName.c_str());
                                    ImGui::Checkbox("IsEnabled", &(compPair.second->m_IsEnabled));

                                    if (compTypeID != updater->GetTypeID())
                                    {
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "TypeID Mismatch");
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "[Compont: %s]",
                                                           compName.c_str());
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "[Updater: %s]",
                                                           updaterTypeName.c_str());
                                    }
                                }
                                else
                                {
                                    ImGui::Text("No components in the updater");
                                    ImGui::Separator();
                                }

                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                        }
                    }
                    ImGui::EndChild();
                }
            }
            ImGui::End();
        }

        if (m_ComponentDebugWindowIsOpen)
        {
            int debugWindowWidth = m_pWindow->GetWidth();
            int debugWindowMaxHeight = m_pWindow->GetHeight();
            // make controls widget width to be 1/4 of the main window width
            if ((debugWindowWidth /= 5) < 200) { debugWindowWidth = 200; }
            ImGui::SetNextWindowSizeConstraints(ImVec2(static_cast<float>(debugWindowWidth), 200), ImVec2(FLT_MAX, static_cast<float>(debugWindowMaxHeight - 20)));

            // create a debugger window for the component
            if (ImGui::Begin("Component", &m_ComponentDebugWindowIsOpen))
            {
                if (!m_DebugTargetComponent.expired())
                {
                    auto component = m_DebugTargetComponent.lock();
                    ImGui::Text("Type: %s", component->GetComponentName().c_str());
                    ImGui::Text("Entity: %s", component->GetEntityPtr()->GetName().c_str());
                    ImGui::SameLine();
                    if (ImGui::Button("goto"))
                    {
                        selectedEntityID = component->GetEntityPtr()->GetID();
                    }

                    ImGui::Separator();

                    auto windowSize = ImGui::GetWindowSize();
                    component->onComponentDebugWindowGUI(windowSize.x, windowSize.y);
                }
                else
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "No component is selected");
                }
            }
            ImGui::End();
        }
#endif
    }

    std::weak_ptr<Entity> SceneLayer::CreateEntity(const std::string& name)
    {
        auto entity = std::make_shared<Entity>(m_EntityIDCounter, name);
        LazyAddComponentToEntity<Transform>(entity);

        auto [entityItr, success] = m_Entities.insert({m_EntityIDCounter, std::move(entity)});
        m_EntityIDCounter++;
        return entityItr->second;
    }

    void SceneLayer::ImmediateDestroyEntity(uint32_t entityID)
    {
        auto entity = GetEntity(entityID);

        SDL_Log("Start Excluding Comps");

        /// Remove components from the updater
        for (const auto& updater : m_ComponentUpdaters)
        {
            updater->RemoveComponentsOfEntity(entityID);
            /// faster remove below
            /*
            /// If the entity has a component updated by this updater, remove it from the updater list
            if (entity->m_Components.find(updater->GetTypeID()) != entity->m_Components.cend())
            {
                updater->RemoveComponentsOfEntity(entityID);
            }*/
        }

        SDL_Log("Start Removing Entity");

        /// Remove entity
        auto entPairItr = m_Entities.find(entityID);
        m_Entities.erase(entPairItr);
    }

    Entity *SceneLayer::GetEntity(uint32_t id)
    {
        auto entItr = m_Entities.find(id);

        if (entItr == m_Entities.end())
        {
            return entItr->second.get();
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