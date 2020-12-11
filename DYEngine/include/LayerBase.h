#pragma once

#include "Events/Event.h"

namespace DYE
{

    class LayerBase
    {
    public:
        /// Ctor
        /// \param name debug purpose name
        LayerBase(const std::string& name = "Layer");
        virtual ~LayerBase() = default;

        /// Called by LayerStack
        virtual void OnAttach() {}

        /// Called by LayerStack
        virtual void OnDetach() {}

        virtual void OnOnUpdate() {}

        virtual void OnImGui() {}

        virtual void OnEvent(const std::shared_ptr<Event>& pEvent) {}

        const std::string& GetName() const { return m_Name; }

    protected:
        std::string m_Name;
    };
}