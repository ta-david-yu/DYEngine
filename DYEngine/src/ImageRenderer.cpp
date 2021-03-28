#include "Scene/ImageRenderer.h"

#ifdef DYE_DEBUG
#include <imgui.h>
#endif

namespace DYE
{
    ImageRenderer::~ImageRenderer()
    {

    }

    void ImageRenderer::OnUpdate()
    {
        /// TODO: Register to updater
    }

#ifdef DYE_DEBUG
    void ImageRenderer::onComponentDebugWindowGUI(float width, float height)
    {
        ComponentBase::onComponentDebugWindowGUI(width, height);

        ImGui::PushItemWidth(width * 0.5f / 3);

        ImGui::Text("Dimension");
        ImGui::DragInt("width", (int*)&m_Width, 1, 0, INT32_MAX);
        ImGui::SameLine(); ImGui::DragInt("height", (int*)&m_Height, 1, 0, INT32_MAX);

        ImGui::PopItemWidth();
    }
#endif

    void ImageRendererUpdater::UpdateComponents()
    {

    }

    void ImageRendererUpdater::FixedUpdateComponents()
    {

    }

    void ImageRendererUpdater::RemoveComponentsOfEntity(uint32_t entityID)
    {

    }

    void ImageRendererUpdater::RenderImages()
    {

    }
}