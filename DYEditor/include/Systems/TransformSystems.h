#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct ComputeLocalToWorldSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Compute Local To World System";

		inline bool ExecuteInEditMode() const final { return true; }
		ExecutionPhase GetPhase() const final { return ExecutionPhase::LateUpdate; }
		void InitializeLoad(DYE::DYEditor::World& world, DYE::DYEditor::InitializeLoadParameters) final;
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
		void DrawInspector(DYE::DYEditor::World &world) final;
	};
}