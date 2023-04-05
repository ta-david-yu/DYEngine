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

        /// Use this to initialize layer states if the layer is pushed after entering Application.Run loop.
        virtual void OnAttach() {}

        /// Use this to release resources owned by the layer.
        virtual void OnDetach() {}

        // Called before game while loop in Application.Run.
        // The event would not get triggerred if the layer is pushed after entering Application.Run loop.
        virtual void OnPreApplicationRun() {}

        virtual void OnFixedUpdate() {}

        virtual void OnUpdate() {}

        virtual void OnRender() {}

		virtual void OnPostRender() {}

        virtual void OnImGui() {}

        virtual void OnEvent(Event& event) {}

        const std::string& GetName() const { return m_Name; }

    protected:
        std::string m_Name;
    };
}