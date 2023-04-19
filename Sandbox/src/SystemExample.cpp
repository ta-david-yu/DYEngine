#include "SystemExample.h"

#include "World.h"
#include "Entity.h"
#include "ImGui/ImGuiUtil.h"
#include "Util/Time.h"
#include "Math/Math.h"

#include "TestComponents.h"
#include "Components.h"
#include "NameComponent.h"

namespace SystemNamespace
{
	void InitializeSystemA::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
	{

	}
}

void UpdateSystemA::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	m_ExecutionCount++;
}

void UpdateSystemA::DrawInspector(DYE::DYEntity::World &world)
{
	DYE::ImGuiUtil::DrawIntControl("Execution Count", m_ExecutionCount);
}

void UpdateSystemB::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	m_ExecutionCount++;
}

void UpdateSystemB::DrawInspector(DYE::DYEntity::World &world)
{
	DYE::ImGuiUtil::DrawIntControl("Update B Exe Count", m_ExecutionCount);
}

void RotateHasAngularVelocitySystem::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	auto& registry = DYE::DYEntity::GetWorldUnderlyingRegistry(world);
	auto view = registry.view<HasAngularVelocity, DYE::DYEntity::TransformComponent>();

	for (auto&& [entity, hasAngularVelocity, transform] : view.each())
	{
		float const radianZ = glm::radians(DYE::TIME.DeltaTime() * hasAngularVelocity.AngleDegreePerSecond);
		transform.Rotation *= glm::quat(glm::vec3{0, 0, radianZ});
	}
}

void CreateEntitiesSystem::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	auto& registry = DYE::DYEntity::GetWorldUnderlyingRegistry(world);
	auto view = registry.view<CreateEntity>();

	for (auto entity : view)
	{
		auto& create = registry.get<CreateEntity>(entity);
		for (int i = 0; i < create.NumberOfEntitiesToCreate; ++i)
		{
			char name[256];
			if (!create.EntityNamePrefix.empty())
			{
				sprintf(name, "%s_Entity_%03d", create.EntityNamePrefix.c_str(), i);
			}
			else
			{
				sprintf(name, "Entity_%03d", i);
			}
			world.CreateEntity(name).AddOrGetComponent<PrintMessageOnTeardown>().Message = std::to_string(i);
		}
		registry.destroy(entity);
	}
}

void PrintMessageOnTeardownSystem::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	auto& registry = DYE::DYEntity::GetWorldUnderlyingRegistry(world);
	auto view = registry.view<DYE::DYEntity::NameComponent, PrintMessageOnTeardown>();

	for (auto&& [entity, name, message] : view.each())
	{
		printf("%s: %s\n", name.Name.c_str(), message.Message.c_str());
	}
}
