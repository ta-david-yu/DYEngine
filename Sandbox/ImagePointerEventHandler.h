#pragma once
#include "Scene/ComponentBase.h"

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <utility>
#include <functional>

namespace DYE
{
    class ImageRenderer;
    class ImagePointerEventHandlerUpdater;
    class WindowBase;

    /// A demo class for receiving mouse event on ImageRenderer
    class ImagePointerEventHandler : public ComponentBase
    {
        friend ImagePointerEventHandlerUpdater;
    public:
        ImagePointerEventHandler() = default;
        ~ImagePointerEventHandler() override = default;

        void OnPointerEnter();
        void OnPointerExit();
        void OnPointerDown();
        void OnPointerUp();

        void OnUpdate() override {}

        std::weak_ptr<ImageRenderer> GetImage() const { return m_Image;}
        void SetImage(std::weak_ptr<ImageRenderer> image) { m_Image = std::move(image); }

        std::function<void(const ImagePointerEventHandler&)> OnPointerEnterCallback;
        std::function<void(const ImagePointerEventHandler&)> OnPointerExitCallback;
        std::function<void(const ImagePointerEventHandler&)> OnPointerDownCallback;
        std::function<void(const ImagePointerEventHandler&)> OnPointerUpCallback;

    private:
        std::weak_ptr<ImageRenderer> m_Image;
    };

    class ImagePointerEventHandlerUpdater : public ComponentUpdaterBase
    {
    public:
        ImagePointerEventHandlerUpdater(ComponentTypeID typeID, WindowBase* window);

        void Init() override;
        void UpdateComponents() override;
        void FixedUpdateComponents() override;
        void HandleOnEvent(Event &event) override;

        void RemoveComponentsOfEntity(uint32_t pointerEventHandler) override;
        void attachEntityWithComponent(const std::weak_ptr<Entity> &entity, const std::shared_ptr<ComponentBase> &component) override;

    private:
        /// Currently drawing target window (get dimension from this window)
        WindowBase* m_pWindow {nullptr};

        /// Cache the image renderer when a image renderer is attached to an entity
        std::vector<std::shared_ptr<ImagePointerEventHandler>> m_CachedPointerEventHandlers;

        /// The event handler the user is currently hovered on
        std::weak_ptr<ImagePointerEventHandler> m_CurrentPointerEventHandler;

        glm::vec<2, int> m_LastMousePosition {0, 0};

    protected:
#if DYE_DEBUG
        void onUpdaterDebugWindowGUI(float width, float height) override;
#endif
    };
}