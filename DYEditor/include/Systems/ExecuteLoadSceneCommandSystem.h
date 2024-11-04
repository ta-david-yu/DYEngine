#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
    struct ExecuteLoadSceneCommandSystem final : public SystemBase
    {
        static constexpr char const *TypeName = "Execute Load Scene Command System";

        ExecutionPhase GetPhase() const override
        {
            return ExecutionPhase::Cleanup;
        }

        void Execute(World &world, DYE::DYEditor::ExecuteParameters params) override;
    };
}