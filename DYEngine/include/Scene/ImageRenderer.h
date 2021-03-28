#pragma once
#include "Scene/ComponentBase.h"

#include <glm/glm.hpp>

namespace DYE
{
    /// ImageRenderer draws the given image texture in 2d screen space
    class ImageRenderer : public ComponentBase
    {
    public:
        ImageRenderer() = default;
        ~ImageRenderer();

        void OnUpdate() override;

        std::uint32_t& GetWidth() { return m_Width; }
        std::uint32_t& GetHeight() { return m_Width; }
        void SetDimension(std::uint32_t width, std::uint32_t height) { m_Width = width; m_Height = height; }
    private:
        std::uint32_t m_Width {100};
        std::uint32_t m_Height {100};

#ifdef DYE_DEBUG
    protected:
        void onComponentDebugWindowGUI(float width, float height) override;
#endif
    };

    class ImageRendererUpdater : ComponentUpdaterBase
    {
    public:
        void UpdateComponents() override;
        void FixedUpdateComponents() override;
        void RemoveComponentsOfEntity(uint32_t entityID) override;

        void RenderImages();
    private:

    };
}