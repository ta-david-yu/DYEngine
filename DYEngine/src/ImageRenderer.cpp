#include "Scene/ImageRenderer.h"

#include "Logger.h"

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

        ImGui::Separator();

        ImGui::Text("Sorting Layer");
        ImGui::DragInt("##sortinglayer", (int*)&m_SortingLayerID, 1, 0, INT32_MAX);

        ImGui::Text("Sorting Order");
        ImGui::DragInt("##sortingorder", (int*)&m_SortingOrder, 1, 0, 0);
    }
#endif

    ImageRendererUpdater::ImageRendererUpdater(ComponentTypeID typeID, WindowBase* window) : ComponentUpdaterBase(typeID), m_pWindow(window)
    {

    }

    void ImageRendererUpdater::UpdateComponents()
    {

    }

    void ImageRendererUpdater::FixedUpdateComponents()
    {

    }

    void ImageRendererUpdater::RemoveComponentsOfEntity(uint32_t entityID)
    {

    }

    void ImageRendererUpdater::attachEntityWithComponent(const std::weak_ptr<Entity> &entity,
                                                         const std::shared_ptr<ComponentBase> &component)
    {
        const auto imageRenderer = std::static_pointer_cast<ImageRenderer>(component);
        imageRenderer->m_pUpdater = this;
    }

    void ImageRendererUpdater::RenderImages()
    {
        /// TODO
    }

    void ImageRendererUpdater::PushSortingLayer(const std::string &layerName)
    {
        m_SortingLayers.push_back(layerName);
    }

    std::uint32_t ImageRendererUpdater::LayerNameToLayerID(const std::string &layerName)
    {
        std::uint32_t id = 0;
        for (const auto& name : m_SortingLayers)
        {
            if (name == layerName)
            {
                return id;
            }
        }

        DYE_LOG("Unknown LayerName - {%s}, return 0 as ID", layerName.c_str());
        return 0;
    }

    std::string ImageRendererUpdater::LayerIDToLayerName(std::uint32_t id)
    {
        /// Out of range, return the first layer name
        if (id >= m_SortingLayers.size())
        {
            DYE_LOG("Unknown LayerID - {%d}, return <unknown layer> as name", id);
            return "<unknown layer>";
        }

        return m_SortingLayers[id];
    }
}
