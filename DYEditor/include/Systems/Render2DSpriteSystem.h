#pragma once

#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	struct Render2DSpriteSystem final : public SystemBase
	{
		static constexpr char const* TypeName = "Render 2D Sprite System";

		inline ExecutionPhase GetPhase() const override { return ExecutionPhase::Render; }
		void InitializeLoad(DYE::DYEntity::World &world, DYE::DYEditor::InitializeLoadParameters) override;
		void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) override;
		void DrawInspector(DYE::DYEntity::World &world) override;

	private:
		int m_NumberOfRenderedEntitiesLastFrame = 0;
	};
}