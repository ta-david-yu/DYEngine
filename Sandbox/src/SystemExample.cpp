#include "SystemExample.h"

#include "World.h"
#include "ImGui/ImGuiUtil.h"

namespace SystemNamespace
{
	void DerivedSystemB::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
	{

	}
}

void DerivedSystemA::Execute(DYE::DYEntity::World &world, DYE::DYEditor::ExecuteParameters params)
{
	m_ExecutionCount++;
}

void DerivedSystemA::DrawInspector(DYE::DYEntity::World &world)
{
	DYE::ImGuiUtil::DrawIntControl("Execution Count", m_ExecutionCount);
}

