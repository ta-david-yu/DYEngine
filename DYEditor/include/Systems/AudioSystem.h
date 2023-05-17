#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct AudioSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Audio System";

		inline ExecutionPhase GetPhase() const override { return ExecutionPhase::Cleanup; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) override;
	};
}