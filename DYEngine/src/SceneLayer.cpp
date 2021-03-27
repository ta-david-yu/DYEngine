#include "Scene/SceneLayer.h"

#include "Base.h"
#include "Time.h"
#include "WindowBase.h"
#include "Logger.h"
#include "Scene/Entity.h"
#include "Scene/Transform.h"
#include "Util/Type.h"
#include "Graphics/Shader.h"

#include <algorithm>
#include <queue>
#include <stack>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>

#include <imgui.h>
#include <glad/glad.h>
#include <SDL.h>
#include <glm/glm.hpp>


namespace DYE
{
    SceneLayer::SceneLayer(WindowBase *pWindow) : m_pWindow(pWindow)
    {
        SetupDefaultUpdaters();

        m_DebugShaderProgram = ShaderProgram::CreateFromFile("Basic", "assets/shaders/Basic.shader");

        m_DebugShaderProgram->Bind();

        // Vertex Buffer
        glm::vec2 positions[3] = {
                glm::vec2{-0.5f, -0.5f},
                glm::vec2{0.0f, 0.5f},
                glm::vec2{0.5f, -0.5f}
        };

        unsigned int bufferId;
        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2), positions, GL_STATIC_DRAW);

        // location (index), count (pos2d now), type (float), stride (the size of the struct), the local location pointer to the attribute (null in our case)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
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

    void SceneLayer::OnRender()
    {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void SceneLayer::OnImGui()
    {
#if DYE_DEBUG
        static uint32_t selectedEntityID = 0;

        /// Force expand tree node of selected entity's parents
        static std::set<uint32_t> forceExpandTreeNodeEntityIDs {};

        auto forceExpandParentsTreeNode = [this](uint32_t entityID)
        {
            if (!m_Entities.empty())
            {
                /// Get the current selected entity and trace its parents
                auto entItr = m_Entities.find(entityID);
                if (entItr != m_Entities.end())
                {
                    auto transform = entItr->second->GetTransform().lock();
                    while (!transform->GetParent().expired())
                    {
                        forceExpandTreeNodeEntityIDs.insert(transform->GetParent().lock()->GetEntityPtr()->GetID());
                        transform = transform->GetParent().lock();
                    }
                }
            }
        };

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

                    if (!m_Entities.empty())
                    {
                        /// If the entity with the id doesn't exist, select the first entity in the list
                        if (m_Entities.find(selectedEntityID) == m_Entities.end())
                        {
                            selectedEntityID = m_Entities.begin()->first;
                        }
                    }
                    else
                    {
                        /// No entity, set to 0
                        selectedEntityID = 0;
                    }

                    /// Entity Hierarchy: Left
                    {
                        ImGui::BeginChild("entity hierarchy", ImVec2(entityHierarchyWidth, entityViewHeight), true);
                        for (const auto &entPair : m_Entities)
                        {
                            auto entity = entPair.second;
                            auto transform = entity->GetTransform();
                            std::stack<Transform*> transformStack;
                            std::stack<int> depthStack;
                            int prevDepth = 0;

                            /// Keep tracks of the depth of the current hierarchy
                            /// Call TreePop() [hierarchyDepth] times afterwards
                            int hierarchyDepth = 0;

                            /// Push the transform into the queue if the parent is null (root transform)
                            if (transform.lock()->GetParent().expired())
                            {
                                transformStack.push(transform.lock().get());
                                depthStack.push(0);
                            }

                            while (!transformStack.empty())
                            {
                                auto currTransform = transformStack.top();
                                auto currDepth = depthStack.top();
                                transformStack.pop();
                                depthStack.pop();

                                if (currDepth < prevDepth)
                                {
                                    hierarchyDepth--;
                                    ImGui::TreePop();
                                }

                                /// We assume transform always has an entity, otherwise it will be super buggy :))
                                auto currEntity = currTransform->GetEntityPtr();

                                ImGui::PushID(currEntity->GetID());
                                {
                                    char entityNodeLabel[128];
                                    sprintf(entityNodeLabel, "##%s", currEntity->GetName().c_str());

                                    bool hasChildren = !currTransform->m_Children.empty();
                                    if (hasChildren)
                                    {
                                        /// Whether or not the tree node (entity) is forced open
                                        if (forceExpandTreeNodeEntityIDs.find(currEntity->GetID()) != forceExpandTreeNodeEntityIDs.end())
                                        {
                                            ImGui::SetNextTreeNodeOpen(true);
                                        }
                                        bool openNode = ImGui::TreeNodeEx(entityNodeLabel);

                                        ImGui::SameLine();

                                        char entityLabel[128];
                                        sprintf(entityLabel, "%s", currEntity->GetName().c_str());
                                        if (ImGui::Selectable(entityLabel, selectedEntityID == currEntity->GetID()))
                                        {
                                            selectedEntityID = currEntity->GetID();
                                        }

                                        if (openNode)
                                        {
                                            for (auto it = currTransform->m_Children.rbegin(); it != currTransform->m_Children.rend(); ++it)
                                            {
                                                transformStack.push(*it);
                                                depthStack.push(currDepth + 1);
                                            }
                                            /// Increase the depth of the hierarchy
                                            hierarchyDepth++;
                                        }
                                    }
                                    else
                                    {
                                        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                                        ImGui::TreeNodeEx("##bulletNode", flags);
                                        ImGui::SameLine();

                                        char entityLabel[128];
                                        sprintf(entityLabel, "%s", currEntity->GetName().c_str());
                                        if (ImGui::Selectable(entityLabel, selectedEntityID == currEntity->GetID()))
                                        {
                                            selectedEntityID = currEntity->GetID();
                                        }
                                    }
                                }
                                ImGui::PopID();

                                prevDepth = currDepth;
                            }

                            while (hierarchyDepth > 0)
                            {
                                hierarchyDepth--;
                                ImGui::TreePop();
                            }
                        }
                        /// Clean up force expand entity IDs list
                        forceExpandTreeNodeEntityIDs.clear();
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
                            ImGui::Text("Has Transform: %s", entity->GetTransform().expired()? "no" : "yes");
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

                                            comp->onComponentDebugWindowGUI(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

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
                    float compListViewWidth = 300;

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
                                    auto compEntity = compPair.second->GetEntityPtr();

                                    char updaterCompLabel[128];
                                    sprintf(updaterCompLabel, "[ID: %d] %s##%d", compEntity->GetID(), compEntity->GetName().c_str(), selectedUpdaterIndex);
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
                                    if (ImGui::Button("goto entity"))
                                    {
                                        selectedEntityID = entPtr->GetID();
                                        forceExpandParentsTreeNode(selectedEntityID);
                                    }
                                    ImGui::Separator();

                                    ImGui::Text("%s", compName.c_str());
                                    if (ImGui::Button("goto component"))
                                    {
                                        m_ComponentDebugWindowIsOpen = true;
                                        m_DebugTargetComponent = compPair.second;
                                    }
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
                        forceExpandParentsTreeNode(selectedEntityID);
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

        /// Cache transform component
        entity->m_Transform = LazyAddComponentToEntity<Transform>(entity);

        auto [entityItr, success] = m_Entities.insert({m_EntityIDCounter, std::move(entity)});
        m_EntityIDCounter++;
        return entityItr->second;
    }

    void SceneLayer::ImmediateDestroyEntity(uint32_t entityID)
    {
        auto entity = GetEntity(entityID);

        DYE_LOG_INFO("Start Excluding Comps");

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

        DYE_LOG_INFO("Start Removing Entity");

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
            DYE_LOG_WARN("A component updater with the given typeID has already been registered - [ID %d]", registeredIndex);
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
            DYE_LOG_WARN("A component updater with the given typeID has already been registered - [ID %d]", registeredIndex);
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