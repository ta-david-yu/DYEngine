#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct RegisterCameraSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Register Camera System";

		inline ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
		void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) override;
		void DrawInspector(DYE::DYEntity::World &world) override;

	private:
		int m_NumberOfRegisteredCamerasLastFrame = 0;
	};
}