#pragma once
#include "Scene/ComponentBase.h"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace DYE
{
    class ImageRendererUpdater;
    class WindowBase;
    class VertexArray;
    class ShaderProgram;
    class Texture2D;

    /// ImageRenderer draws the given image texture in 2d screen space
    class ImageRenderer : public ComponentBase
    {
        friend ImageRendererUpdater;
    public:
        ImageRenderer() = default;
        ~ImageRenderer();

        void OnUpdate() override;

        std::uint32_t& GetWidth() { return m_Width; }
        std::uint32_t& GetHeight() { return m_Height; }
        void SetDimension(std::uint32_t width, std::uint32_t height) { m_Width = width; m_Height = height; }

        /// Set the texture
        /// \param texture
        /// \param setDimension if automatically change the dimension based on the texture width/height
        void SetTexture(std::shared_ptr<Texture2D> texture, bool setDimension = true);
    private:
        glm::vec4 m_Color {1, 1, 1, 1};
        std::shared_ptr<Texture2D> m_Texture;

        std::uint32_t m_Width {100};
        std::uint32_t m_Height {100};

        std::uint32_t m_SortingLayerID {0};
        int m_SortingOrder {0};

        ImageRendererUpdater* m_pUpdater {nullptr};

#ifdef DYE_DEBUG
    protected:
        void onComponentDebugWindowGUI(float width, float height) override;
#endif
    };

    class ImageRendererUpdater : public ComponentUpdaterBase
    {
    public:
        explicit ImageRendererUpdater(ComponentTypeID typeID, WindowBase* window);

        void Init() override;
        void UpdateComponents() override;
        void FixedUpdateComponents() override;
        void RemoveComponentsOfEntity(uint32_t entityID) override;
        void attachEntityWithComponent(const std::weak_ptr<Entity> &entity, const std::shared_ptr<ComponentBase> &component) override;

        /// Called in Render update, render the registered enabled images (in sorted order)
        void RenderImages();

        void PushSortingLayer(const std::string& layerName);

        /// Get the layer id of the given layer name
        /// \param layerName
        /// \return the id of the layer with the given name, return 0 if no layer with the name is found
        std::uint32_t LayerNameToLayerID(const std::string& layerName);

        /// Get the layer name of the given layer id
        /// \param id
        /// \return the name of the layer with the given id, return "<unknown layer>" if no layer with the id is found
        std::string LayerIDToLayerName(std::uint32_t id);

        std::vector<std::string>& GetSortingLayers() { return m_SortingLayers; }

        void MarkDirty() { m_IsDirty = true; }
    private:
        /// Currently drawing target window (get dimension from this window)
        WindowBase* m_pWindow {nullptr};
        std::vector<std::string> m_SortingLayers { "Default" };

        /// Cache the image renderer when a image renderer is attached to an entity
        std::vector<std::shared_ptr<ImageRenderer>> m_CachedImageRenderers;

        std::shared_ptr<VertexArray> m_QuadVertexArray {nullptr};
        std::shared_ptr<ShaderProgram> m_DefaultShaderProgram {nullptr};
        std::shared_ptr<Texture2D> m_DefaultTexture2D {nullptr};

        /// Whether or not the images should be sorted before render
        bool m_IsDirty = true;

    protected:
#if DYE_DEBUG
        void onUpdaterDebugWindowGUI(float width, float height) override;
#endif
    };
}