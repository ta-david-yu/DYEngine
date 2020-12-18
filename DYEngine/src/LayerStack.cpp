#include "LayerStack.h"

#include "LayerBase.h"

#include <algorithm>

namespace DYE
{
    LayerStack::~LayerStack()
    {
        for (const std::shared_ptr<LayerBase>& layer : m_Layers)
        {
            layer->OnDetach();
        }
    }

    void LayerStack::PushLayer(std::shared_ptr<LayerBase> layer)
    {
        layer->OnAttach();
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, std::move(layer));
        m_LayerInsertIndex++;
    }

    void LayerStack::PopLayer(std::shared_ptr<LayerBase>& layer)
    {
        auto layerEnd = m_Layers.begin() + m_LayerInsertIndex;
        auto it  = std::find(m_Layers.begin(), layerEnd, layer);

        if (it != layerEnd)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_LayerInsertIndex--;
        }
    }

    void LayerStack::PushOverlay(std::shared_ptr<LayerBase> overlay)
    {
        overlay->OnAttach();
        m_Layers.emplace_back(overlay);
    }

    void LayerStack::PopOverlay(std::shared_ptr<LayerBase> &overlay)
    {
        auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
        if (it != m_Layers.end())
        {
            overlay->OnDetach();
            m_Layers.erase(it);
        }
    }
}