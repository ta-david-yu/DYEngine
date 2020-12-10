#pragma once

#include <vector>

namespace DYE
{
    class LayerBase;

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        /// call OnAttach, push a layer on the top of layer but below the overlay
        /// \param layer
        void PushLayer(std::shared_ptr<LayerBase> layer);
        void PopLayer(std::shared_ptr<LayerBase>& layer);

        /// call OnAttach, push a layer on the top of the stack (overlay)
        /// \param layer
        void PushOverlay(std::shared_ptr<LayerBase> overlay);
        void PopOverlay(std::shared_ptr<LayerBase>& overlay);

        std::vector<std::shared_ptr<LayerBase>>::iterator begin() { return m_Layers.begin(); }
        std::vector<std::shared_ptr<LayerBase>>::iterator end() { return m_Layers.end(); }
        std::vector<std::shared_ptr<LayerBase>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        std::vector<std::shared_ptr<LayerBase>>::reverse_iterator rend() { return m_Layers.rend(); }

        std::vector<std::shared_ptr<LayerBase>>::const_iterator begin() const { return m_Layers.begin(); }
        std::vector<std::shared_ptr<LayerBase>>::const_iterator end() const { return m_Layers.end(); }
        std::vector<std::shared_ptr<LayerBase>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
        std::vector<std::shared_ptr<LayerBase>>::const_reverse_iterator rend() const { return m_Layers.rend(); }

    private:
        std::vector<std::shared_ptr<LayerBase>> m_Layers;
        std::uint32_t m_LayerInsertIndex = 0;
    };
}