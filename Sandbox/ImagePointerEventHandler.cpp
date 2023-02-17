#include <ImagePointerEventHandler.h>

#include <Scene/GameObject.h>
#include <Scene/Transform.h>
#include <Scene/ImageRenderer.h>
#include <Event/Event.h>
#include <Event/MouseEvent.h>
#include "Graphics/WindowBase.h"

#include "Util/Macro.h"

#ifdef DYE_DEBUG
#include <imgui.h>
#endif

#include <algorithm>

namespace DYE
{
    void ImagePointerEventHandler::OnPointerEnter()
    {
        DYE_LOG("Enter %s", GetEntityPtr()->GetName().c_str());
        if (OnPointerEnterCallback)
            OnPointerEnterCallback(*this);
    }

    void ImagePointerEventHandler::OnPointerExit()
    {
        DYE_LOG("Exit %s", GetEntityPtr()->GetName().c_str());
        if (OnPointerExitCallback)
            OnPointerExitCallback(*this);
    }

    void ImagePointerEventHandler::OnPointerDown()
    {
        DYE_LOG("Down %s", GetEntityPtr()->GetName().c_str());
        if (OnPointerDownCallback)
            OnPointerDownCallback(*this);
    }

    void ImagePointerEventHandler::OnPointerUp()
    {
        DYE_LOG("Up %s", GetEntityPtr()->GetName().c_str());
        if (OnPointerUpCallback)
            OnPointerUpCallback(*this);
    }

    ImagePointerEventHandlerUpdater::ImagePointerEventHandlerUpdater(ComponentTypeID typeID, WindowBase* window)
            : ComponentUpdaterBase(typeID), m_pWindow(window)
    {

    }

    void ImagePointerEventHandlerUpdater::Init()
    {

    }

    void ImagePointerEventHandlerUpdater::UpdateComponents()
    {

    }

    void ImagePointerEventHandlerUpdater::FixedUpdateComponents()
    {

    }

    void ImagePointerEventHandlerUpdater::HandleOnEvent(Event &event)
    {
        if (event.IsInCategory(EventCategory::Mouse))
        {
            if (event.GetEventType() == EventType::MouseButtonDown)
            {
                if (!m_CurrentPointerEventHandler.expired())
                {
                    m_CurrentPointerEventHandler.lock()->OnPointerDown();
                }
            }
            else if (event.GetEventType() == EventType::MouseButtonUp)
            {
                if (!m_CurrentPointerEventHandler.expired())
                {
                    m_CurrentPointerEventHandler.lock()->OnPointerUp();
                }
            }
            else if (event.GetEventType() == EventType::MouseMove)
            {
                auto& mouseMovedEvent = static_cast<MouseMovedEvent&>(event);
                m_LastMousePosition = {mouseMovedEvent.GetX(), mouseMovedEvent.GetY()};

                /// Sort first, front to back
                struct {
                    bool operator()(const std::shared_ptr<ImagePointerEventHandler>& a, std::shared_ptr<ImagePointerEventHandler>& b) const
                    {
                        auto imageA = a->GetImage();
                        auto imageB = b->GetImage();
                        if (imageA.lock()->GetSortingLayerID() > imageB.lock()->GetSortingLayerID())
                        {
                            return true;
                        }
                        else if (imageA.lock()->GetSortingLayerID() < imageB.lock()->GetSortingLayerID())
                        {
                            return false;
                        }
                        else
                        {
                            return imageA.lock()->GetSortingOrder() > imageB.lock()->GetSortingOrder();
                        }
                    }
                } customSort;
                std::sort(m_CachedPointerEventHandlers.begin(), m_CachedPointerEventHandlers.end(), customSort);

                /// The current pointer event handler from the last event
                auto prevEventHandler = m_CurrentPointerEventHandler;
                std::weak_ptr<ImagePointerEventHandler> newEventHandler;
                /// Rect testing from the image at the front to the one at the back
                for (const auto& pointerEventHandler : m_CachedPointerEventHandlers)
                {
                    if (pointerEventHandler->GetEntityPtr()->IsActive() && pointerEventHandler->GetIsEnabled())
                    {
                        auto pTransform = pointerEventHandler->GetTransform();
                        auto image = pointerEventHandler->GetImage();

                        glm::vec3 worldPos = pTransform->GetLocalPosition();
                        glm::vec<2, int> anchorPos{worldPos.x, worldPos.y};

                        auto relativePos = glm::vec<2, int>{mouseMovedEvent.GetX(), mouseMovedEvent.GetY()} - anchorPos;

                        glm::vec3 worldScale = pTransform->GetLocalScale();
                        auto imageDimension = image.lock()->GetDimension();

                        decltype(imageDimension) scaledDimension { worldScale.x * imageDimension.x,
                                                                 worldScale.y * imageDimension.y };
                        int halfWidth = scaledDimension.x / 2;
                        int halfHeight = scaledDimension.y / 2;

                        /// If it's inside an image?
                        if (relativePos.x <= halfWidth && relativePos.x >= -halfWidth &&
                            relativePos.y <= halfHeight && relativePos.y >= -halfHeight)
                        {
                            newEventHandler = pointerEventHandler;
                            break;
                        }
                    }
                }

                if (prevEventHandler.expired())
                {
                    /// null to new
                    if (!newEventHandler.expired())
                        newEventHandler.lock()->OnPointerEnter();
                }
                else if (!prevEventHandler.expired())
                {
                    /// old to null
                    if (newEventHandler.expired())
                        prevEventHandler.lock()->OnPointerExit();
                    /// old to another one
                    else if (!newEventHandler.expired())
                    {
                        /// old to new
                        if (prevEventHandler.lock().get() != newEventHandler.lock().get())
                        {
                            prevEventHandler.lock()->OnPointerExit();
                            newEventHandler.lock()->OnPointerEnter();
                        }
                    }
                }

                m_CurrentPointerEventHandler = newEventHandler;
            }
        }
    }

    void ImagePointerEventHandlerUpdater::RemoveComponentsOfEntity(uint32_t entityID)
    {
        m_CachedPointerEventHandlers.erase(std::remove_if(m_CachedPointerEventHandlers.begin(), m_CachedPointerEventHandlers.end(),
                                                    [entityID](const auto& pointerEventHandler)
                                                    {
                                                        return pointerEventHandler->GetEntityPtr()->GetID() == entityID;
                                                    }), m_CachedPointerEventHandlers.end());
    }

    void ImagePointerEventHandlerUpdater::attachEntityWithComponent(const std::weak_ptr<Entity> &entity,
                                                                    const std::shared_ptr<ComponentBase> &component)
    {
        const auto pointerEventHandler = std::static_pointer_cast<ImagePointerEventHandler>(component);
        m_CachedPointerEventHandlers.push_back(pointerEventHandler);
    }

#ifdef DYE_DEBUG
    void ImagePointerEventHandlerUpdater::onUpdaterDebugWindowGUI(float width, float height)
    {
        ComponentUpdaterBase::onUpdaterDebugWindowGUI(width, height);

        ImVec4 enabledTextColor {1, 1, 1, 1};
        ImVec4 disabledTextColor {0.5, 0.5, 0.5, 1};

        ImGui::Text("pos: %d, %d", m_LastMousePosition.x, m_LastMousePosition.y);

        ImGui::Separator();
        ImGui::Text("Current PointerEventHandler: ");
        ImGui::SameLine();
        ImGui::TextColored(m_CurrentPointerEventHandler.expired()? disabledTextColor : enabledTextColor,
                           "%s", m_CurrentPointerEventHandler.expired()?
                                "<null>" : m_CurrentPointerEventHandler.lock()->GetEntityPtr()->GetName().c_str() );

        ImGui::Separator();
        ImGui::Text("All Cached Handlers");
        ImGui::BeginChild("cached handlers", ImVec2(width - 20, 0), true);
        for (const auto& handler : m_CachedPointerEventHandlers)
        {
            glm::vec3 worldPos = handler->GetTransform()->GetLocalPosition();
            glm::vec<2, int> anchorPos { worldPos.x, worldPos.y };
            auto relativePos = glm::vec<2, int>{m_LastMousePosition.x, m_LastMousePosition.y} - anchorPos;

            glm::vec3 worldScale = handler->GetTransform()->GetLocalScale();
            auto imageDimension = handler->GetImage().lock()->GetDimension();

            decltype(imageDimension) scaledDimension {worldScale.x * imageDimension.x, worldScale.y * imageDimension.y };
            uint32_t halfWidth = scaledDimension.x / 2;
            uint32_t halfHeight = scaledDimension.y / 2;

            ImGui::PushStyleColor(ImGuiCol_Text, handler->GetIsEnabled() ? enabledTextColor : disabledTextColor);
            ImGui::Text("%s: ", handler->GetEntityPtr()->GetName().c_str());
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::Text("[%d, %d], [%d, %d]", relativePos.x, relativePos.y, -halfWidth, -halfHeight);
        }
        ImGui::EndChild();
    }
#endif
}
