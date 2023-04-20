#include "Systems/Render2DSpriteSystem.h"

#include "Components/TransformComponent.h"
#include "Components/SpriteRendererComponent.h"
#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"
#include "ImGui/ImGuiUtil.h"

#include <string>

namespace DYE::DYEditor
{
	using namespace DYE::DYEditor;

	void Render2DSpriteSystem::InitializeLoad(DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters)
	{
		auto& registry = DYEditor::GetWorldUnderlyingRegistry(world);

		// Call this on initialize load to perform initialization step on the group.
		auto group = registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
	}

	void Render2DSpriteSystem::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
	{
		m_NumberOfRenderedEntitiesLastFrame = 0;
		auto& registry = DYEditor::GetWorldUnderlyingRegistry(world);

		// We use group here because we know Render2DSpriteSystem is the main critical path for SpriteRendererComponent.
		auto group = registry.group<SpriteRendererComponent>(entt::get<TransformComponent>);
		for (auto entity : group)
		{
			auto [sprite, transform] = group.get<SpriteRendererComponent, TransformComponent>(entity);

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

	void Render2DSpriteSystem::DrawInspector(DYEditor::World &world)
	{
		ImGuiUtil::DrawReadOnlyTextWithLabel("Number Of Rendered Entities Last Frame", std::to_string(m_NumberOfRenderedEntitiesLastFrame));
	}
}
