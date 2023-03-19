#pragma once

#include "EditorCore.h"

DYE_SYSTEM("Derived System A", DerivedSystemA)
struct DerivedSystemA final : public DYE::DYEditor::SystemBase
{
	inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Update; }
	void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) final;
	void DrawInspector(DYE::DYEntity::World &world) final;

private:
	int m_ExecutionCount = 0;
};

namespace SystemNamespace
{
	DYE_SYSTEM("Derived System B", SystemNamespace::DerivedSystemB)
	struct DerivedSystemB final : public DYE::DYEditor::SystemBase
	{
		inline DYE::DYEditor::ExecutionPhase GetPhase() const override { return DYE::DYEditor::ExecutionPhase::Initialize; }
		void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) final;
	};
}

