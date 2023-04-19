#pragma once

#include "Core/LayerBase.h"

#include "Core/Scene.h"

namespace DYE::DYEditor
{
	class SceneRuntimeLayer final : public DYE::LayerBase
	{
	public:
		SceneRuntimeLayer();
		~SceneRuntimeLayer() override = default;

		void LoadSceneInstant();

		void OnFixedUpdate() override;
		void OnUpdate() override;
		void OnRender() override;
		void OnPostRender() override;
		void OnImGui() override;

	public:
		DYEditor::Scene ActiveMainScene;
	};
}