#pragma once

#include "Event/Event.h"

#include <memory>

namespace DYE
{
    class Application;

    class LayerBase
    {
        friend Application;
    public:
        /// Ctor
        /// \param name debug purpose name
        explicit LayerBase(std::string name = "Layer");
        virtual ~LayerBase() = default;

        /// Called by LayerStack
        virtual void OnAttach() {}

        /// Called by LayerStack
        virtual void OnDetach() {}

        /// Called before Application.Run loop
        virtual void OnInit() {}

        virtual void OnFixedUpdate() {}

        virtual void OnUpdate() {}

        virtual void OnRender() {}

        virtual void OnImGui() {}

        virtual void OnEvent(const std::shared_ptr<Event>& pEvent) {}

        const std::string& GetName() const { return m_Name; }

    protected:
        std::string m_Name;
    };
}