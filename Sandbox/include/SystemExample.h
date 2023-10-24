#pragma once

#include "Core/EditorCore.h"

DYE_SYSTEM(UpdateSystemA, "Update System A")
struct UpdateSystemA final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Update; }
	void InitializeLoad(DYE::DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters) override
	{
		m_ExecutionCount = 0;
	}
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	void DrawInspector(DYE::DYEditor::World &world) final;

private:
	int m_ExecutionCount = 0;
};

DYE_SYSTEM(UpdateSystemB, "Update System B")
DYE_FORMERLY_KNOWN_AS("Update System B Old Name")
struct UpdateSystemB final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Update; }
	void InitializeLoad(DYE::DYEditor::World &world, DYE::DYEditor::InitializeLoadParameters) override
	{
		m_ExecutionCount = 0;
	}
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	void DrawInspector(DYE::DYEditor::World &world) final;

private:
	int m_ExecutionCount = 0;
};

DYE_SYSTEM(FixedUpdateSystem1, "Fixed Update System 1")
DYE_FORMERLY_KNOWN_AS("Fixed Update System 1 Old Name")
struct FixedUpdateSystem1 final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::FixedUpdate; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final { }
};

DYE_SYSTEM(FixedUpdateSystem2, "Fixed Update System 2")
struct FixedUpdateSystem2 final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::FixedUpdate; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final { }
};

DYE_SYSTEM(FixedUpdateSystem3, "Fixed Update System 3")
struct FixedUpdateSystem3 final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::FixedUpdate; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final { }
};

DYE_SYSTEM("ImGui System 1", ImGuiSystem1)
struct ImGuiSystem1 final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::ImGui; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

namespace SystemNamespace
{
	DYE_SYSTEM(SystemNamespace::InitializeSystemA, "Initialize System A")
	struct InitializeSystemA final : public DYE::DYEditor::SystemBase
	{
		inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Initialize; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};
}

DYE_SYSTEM(RotateHasAngularVelocitySystem, "Rotate Has Angular Velocity System")
struct RotateHasAngularVelocitySystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Update ; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

DYE_SYSTEM(CreateEntitiesSystem, "Create Entities System")
struct CreateEntitiesSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Initialize ; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

DYE_SYSTEM(PrintMessageOnTeardownSystem, "Print Message On Teardown System")
struct PrintMessageOnTeardownSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::TearDown ; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

DYE_SYSTEM(PressButtonToLoadSceneImGuiSystem, "Runtime Load Scene ImGui System")
struct PressButtonToLoadSceneImGuiSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::ImGui; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
private:
	std::filesystem::path m_ScenePath = "";
};

DYE_SYSTEM(GetViewTestImGuiSystem, "Get View Test ImGui System")
struct GetViewTestImGuiSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::ImGui; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

