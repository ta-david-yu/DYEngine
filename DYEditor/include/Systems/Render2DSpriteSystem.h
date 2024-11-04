#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
    struct Render2DSpriteSystem final : public SystemBase
    {
        static constexpr char const *TypeName = "Render 2D Sprite System";

        inline bool ExecuteInEditMode() const final { return true; }
        inline ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
        void InitializeLoad(DYE::DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters) override;
        void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override;
        void DrawInspector(DYE::DYEditor::World &world) override;

    private:
        int m_NumberOfRenderedEntitiesLastFrame = 0;
    };
}