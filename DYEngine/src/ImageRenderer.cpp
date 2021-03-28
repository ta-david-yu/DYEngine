#include "Scene/ImageRenderer.h"

#include "Scene/Entity.h"
#include "Logger.h"

#ifdef DYE_DEBUG
#include <imgui.h>
#endif

#include <algorithm>

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
        const auto& layers = m_pUpdater->GetSortingLayers();

        char previewLayerName[128];
        sprintf(previewLayerName, "%2d: %s", m_SortingLayerID, m_pUpdater->LayerIDToLayerName(m_SortingLayerID).c_str());

        bool isDropdownOpen = ImGui::BeginCombo("##sortingLayer", previewLayerName);
        if (isDropdownOpen)
        {
            std::uint32_t index = 0;
            char itemLayerName[128];
            for (const auto& layerName : layers)
            {
                sprintf(itemLayerName, "%2d: %s##%d", index, m_pUpdater->LayerIDToLayerName(index).c_str(), GetEntityPtr()->GetID());

                bool isSelected = (index == m_SortingLayerID);
                if (ImGui::Selectable(itemLayerName, isSelected))
                    m_SortingLayerID = index;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();

                index++;
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Sorting Order In Layer");
        ImGui::DragInt("##sortingOrder", (int*)&m_SortingOrder, 0.1f, 0, 0);

        ImGui::SameLine();
        if (ImGui::Button("<"))
            m_SortingOrder--;
        ImGui::SameLine();
        if (ImGui::Button(">"))
            m_SortingOrder++;
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
        if (std::find(m_SortingLayers.begin(), m_SortingLayers.end(), layerName) != std::end(m_SortingLayers))
        {
            DYE_LOG("A sorting layer with the name <%s> already exists!", layerName.c_str());
        }
        else
        {
            m_SortingLayers.push_back(layerName);
        }
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

        return 0;
    }

    std::string ImageRendererUpdater::LayerIDToLayerName(std::uint32_t id)
    {
        /// Out of range, return the first layer name
        if (id >= m_SortingLayers.size())
        {
            return "<unknown layer>";
        }

        return m_SortingLayers[id];
    }
}
