#include "Systems/Render2DSpriteSystem.h"

#include "Components/LocalTransformComponent.h"
#include "Components/SpriteRendererComponent.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"
#include "ImGui/ImGuiUtil.h"
#include "SceneViewEntitySelection.h"

#include <string>

namespace DYE::DYEditor
{
	using namespace DYE::DYEditor;

	void Render2DSpriteSystem::InitializeLoad(DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters)
	{
		// Call this on initialize load to perform initialization step on the group.
		auto group = world.GetRegistry().group<SpriteRendererComponent>(Get<LocalTransformComponent>);
	}

	void Render2DSpriteSystem::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
	{
		m_NumberOfRenderedEntitiesLastFrame = 0;
		// We use group here because we know Render2DSpriteSystem is the main critical path for SpriteRendererComponent.
		auto group = world.GetRegistry().group<SpriteRendererComponent>(Get<LocalTransformComponent>);
		for (auto entity : group)
		{
			auto [sprite, transform] = group.get<SpriteRendererComponent, LocalTransformComponent>(entity);

			if (!sprite.IsEnabled)
			{
				continue;
			}

			Entity wrappedEntity = world.WrapIdentifierIntoEntity(entity);

			glm::mat4 modelMatrix = transform.GetTransformMatrix();

			// Scale the matrix based on sprite pixels per unit.
			modelMatrix = glm::scale(modelMatrix, sprite.Texture->GetScaleFromTextureDimensions());

			MaterialPropertyBlock materialPropertyBlock;
			materialPropertyBlock.SetTexture("_MainTex", sprite.Texture);
			materialPropertyBlock.SetFloat4("_MainTex_TilingOffset", {1, 1, 0, 0});
			materialPropertyBlock.SetFloat4("_Color", sprite.Color);

			RenderPipeline2D *pipeline2D = RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>();
			auto geometryVAO = pipeline2D->GetDefaultQuadSpriteVAO();
			pipeline2D->Submit(geometryVAO, pipeline2D->GetDefaultSpriteMaterial(), modelMatrix, materialPropertyBlock);

#ifdef DYE_EDITOR
			// We only submit the sprite to the scene view selection system if it's in the editor.
			SceneViewEntitySelection::RegisterEntityGeometry(wrappedEntity.GetInstanceID(), geometryVAO, modelMatrix);
#endif
			m_NumberOfRenderedEntitiesLastFrame++;
		}
	}

	void Render2DSpriteSystem::DrawInspector(DYEditor::World &world)
	{
		ImGui::Text("Rendered Entities: %d", m_NumberOfRenderedEntitiesLastFrame);
	}
}
