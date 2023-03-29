#pragma once

#include "EditorCore.h"

DYE_SYSTEM("Update System A", UpdateSystemA)
struct UpdateSystemA final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Update; }
	void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) final;
	void DrawInspector(DYE::DYEntity::World &world) final;

private:
	int m_ExecutionCount = 0;
};

DYE_SYSTEM("Fixed Update System B", FixedUpdateSystemB)
struct FixedUpdateSystemB final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::FixedUpdate; }
	void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) final { }
};

namespace SystemNamespace
{
	DYE_SYSTEM("Initialize System A", SystemNamespace::InitializeSystemA)
	struct InitializeSystemA final : public DYE::DYEditor::SystemBase
	{
		inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Initialize; }
		void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};
}

