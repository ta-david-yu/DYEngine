#pragma once

#include "Core/EditorCore.h"

DYE_SYSTEM("Update System A", UpdateSystemA)
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

DYE_SYSTEM("Update System B", UpdateSystemB)
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

DYE_SYSTEM("Fixed Update System 1", FixedUpdateSystem1)
struct FixedUpdateSystem1 final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::FixedUpdate; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final { }
};

DYE_SYSTEM("Fixed Update System 2", FixedUpdateSystem2)
struct FixedUpdateSystem2 final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::FixedUpdate; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final { }
};

DYE_SYSTEM("Fixed Update System 3", FixedUpdateSystem3)
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
	DYE_SYSTEM("Initialize System A", SystemNamespace::InitializeSystemA)
	struct InitializeSystemA final : public DYE::DYEditor::SystemBase
	{
		inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Initialize; }
		void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};
}

DYE_SYSTEM("Rotate Has Angular Velocity System", RotateHasAngularVelocitySystem)
struct RotateHasAngularVelocitySystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Update ; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

DYE_SYSTEM("Create Entities System", CreateEntitiesSystem)
struct CreateEntitiesSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Initialize ; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

DYE_SYSTEM("Print Message On Teardown System", PrintMessageOnTeardownSystem)
struct PrintMessageOnTeardownSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::TearDown ; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

DYE_SYSTEM("Runtime Load Scene ImGui System", PressButtonToLoadSceneImGuiSystem)
struct PressButtonToLoadSceneImGuiSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::ImGui; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
private:
	std::filesystem::path m_ScenePath = "";
};

DYE_SYSTEM("Get View Test ImGui System", GetViewTestImGuiSystem)
struct GetViewTestImGuiSystem final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::ImGui; }
	void Execute(DYE::DYEditor::World &world, DYE::DYEditor::ExecuteParameters params) final;
};

