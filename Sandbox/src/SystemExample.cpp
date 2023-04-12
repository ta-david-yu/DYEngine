#include "SystemExample.h"

#include "World.h"
#include "ImGui/ImGuiUtil.h"
#include "TestComponents.h"
#include "Components.h"
#include "Util/Time.h"
#include "Math/Math.h"

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

	for (auto entity : view)
	{
		auto [hasAngularVelocity, transform] = view.get<HasAngularVelocity, DYE::DYEntity::TransformComponent>(entity);
		float const radianZ = glm::radians(DYE::TIME.DeltaTime() * hasAngularVelocity.AngleDegreePerSecond);
		transform.Rotation *= glm::quat(glm::vec3{0, 0, radianZ});
	}
}
