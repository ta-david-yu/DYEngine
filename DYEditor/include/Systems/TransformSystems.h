#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct ComputeLocalToWorldSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Compute Local To World System";

		[[nodiscard]]
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Render; }

		void InitializeLoad(DYE::DYEditor::World& world, DYE::DYEditor::InitializeLoadParameters) final;
		void Execute(DYE::DYEditor::World &childEntityIdentifier, DYE::DYEditor::ExecuteParameters params) final;
		void DrawInspector(DYE::DYEditor::World &world) final;
	};
}