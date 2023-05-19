#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct CreateWindowOnInitializeSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Create Window On Initialize System";

		[[nodiscard]]
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Initialize; }

		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};

	/// In charge of creating, closing, setting position & size, change title etc.
	struct ModifyWindowSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Modify Window System";

		[[nodiscard]]
		ExecutionPhase GetPhase() const final { return ExecutionPhase::Cleanup; }

		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};

	struct CloseWindowOnTearDownSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Close Window On TearDown System";

		[[nodiscard]]
		ExecutionPhase GetPhase() const final { return ExecutionPhase::TearDown; }

		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};
}