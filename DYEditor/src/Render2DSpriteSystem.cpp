#include "Systems/Render2DSpriteSystem.h"

#include "TransformComponent.h"
#include "Components/SpriteRendererComponent.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"
#include "ImGui/ImGuiUtil.h"

#include <string>

namespace DYE::DYEditor
{
	void Render2DSpriteSystem::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
	{
		m_NumberOfRenderedEntitiesLastFrame = 0;
		auto& registry = DYEntity::GetWorldUnderlyingRegistry(world);
		auto view = registry.view<SpriteRendererComponent, DYEntity::TransformComponent>();
		for (auto entity : view)
		{
			auto [sprite, transform] = view.get<SpriteRendererComponent, DYEntity::TransformComponent>(entity);

			if (!sprite.IsEnabled)
			{
				continue;
			}

			glm::mat4 modelMatrix = glm::mat4 {1.0f};
			modelMatrix = glm::translate(modelMatrix, transform.Position);
			modelMatrix = modelMatrix * glm::toMat4(transform.Rotation);
			modelMatrix = glm::scale(modelMatrix, transform.Scale);

			RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()->SubmitSprite(sprite.Texture, sprite.Color, modelMatrix);

			m_NumberOfRenderedEntitiesLastFrame++;
		}
	}

	void Render2DSpriteSystem::DrawInspector(DYEntity::World &world)
	{
		ImGuiUtil::DrawReadOnlyTextWithLabel("Number Of Rendered Entities Last Frame", std::to_string(m_NumberOfRenderedEntitiesLastFrame));
	}
}