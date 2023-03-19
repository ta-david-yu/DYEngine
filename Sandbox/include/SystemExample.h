#pragma once

#include "EditorCore.h"

namespace DYE::DYEntity
{
	class World;
}

namespace SystemNamespace
{
	DYE_SYSTEM_FUNCTION("Initialize Velocity", SystemNamespace::UpdateMovementSystemExample)
	void UpdateMovementSystemExample(DYE::DYEntity::World& world);
}

DYE_SYSTEM_FUNCTION("Initialize Velocity", InitializeVelocitySystemExample)
void InitializeVelocitySystemExample(DYE::DYEntity::World& world);

DYE_SYSTEM("Derived System", DerivedSystem)
struct DerivedSystem final : public DYE::DYEditor::SystemBase
{
	void Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params) final;
	//void DrawInspector(DYE::World &world, DYE::DYEditor::ExecuteParameters params) final;

private:
	int frameCounter = 0;
};
