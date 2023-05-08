#include "SystemExample.h"

#include "Core/World.h"
#include "Core/Entity.h"
#include "ImGui/ImGuiUtil.h"
#include "Core/Time.h"
#include "Math/Math.h"

#include "TestComponents.h"
#include "Core/Components.h"
#include "Components/NameComponent.h"

namespace SystemNamespace
{
	void InitializeSystemA::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
	{

	}
}

void UpdateSystemA::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
{
	m_ExecutionCount++;
}

void UpdateSystemA::DrawInspector(DYE::DYEditor::World &world)
{
	DYE::ImGuiUtil::DrawIntControl("Execution Count", m_ExecutionCount);
}

void UpdateSystemB::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
{
	m_ExecutionCount++;
}

void UpdateSystemB::DrawInspector(DYE::DYEditor::World &world)
{
	DYE::ImGuiUtil::DrawIntControl("Update B Exe Count", m_ExecutionCount);
}

void ImGuiSystem1::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
{
	ImGui::SetNextWindowSize({200, 100}, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("ImGui System 1 Window"))
	{
		ImGui::Text("MSVC TEST");
	}

	ImGui::End();
}

void RotateHasAngularVelocitySystem::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
{
	auto view = world.GetView<HasAngularVelocity, DYE::DYEditor::TransformComponent>();

	for (auto&& [entity, hasAngularVelocity, transform] : view.each())
	{
		float const radianZ = glm::radians(DYE::TIME.DeltaTime() * hasAngularVelocity.AngleDegreePerSecond);
		transform.Rotation *= glm::quat(glm::vec3{0, 0, radianZ});
	}
}

void CreateEntitiesSystem::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
{
	auto view = world.GetView<CreateEntity>();

	for (auto entity : view)
	{
		auto& create = view.get<CreateEntity>(entity);
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
		world.DestroyEntityAndChildren(world.WrapIdentifierIntoEntity(entity));
	}
}

void PrintMessageOnTeardownSystem::Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params)
{
	auto view = world.GetView<DYE::DYEditor::NameComponent, PrintMessageOnTeardown>();

	for (auto&& [entity, name, message] : view.each())
	{
		printf("%s: %s\n", name.Name.c_str(), message.Message.c_str());
	}
}
