#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct ComputeLocalToWorldSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Compute Local To World System";

		[[nodiscard]]
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Render; }

		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};
}