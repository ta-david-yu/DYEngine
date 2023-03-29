#include "SystemExample.h"

#include "World.h"
#include "ImGui/ImGuiUtil.h"

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
