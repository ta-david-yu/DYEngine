#include "Undo/UndoOperationBase.h"

#include "ImGui/ImGuiUtil.h"

namespace DYE::DYEditor
{
	void GroupUndoOperation::Undo()
	{
		for (int i = OperationCollection.size() - 1; i >= 0; --i)
		{
			OperationCollection[i]->Undo();
		}
	}

	void GroupUndoOperation::Redo()
	{
		for (int i = 0; i < OperationCollection.size(); ++i)
		{
			OperationCollection[i]->Redo();
		}
	}

	void GroupUndoOperation::DrawTooltip() const
	{
		for (auto const &operation : OperationCollection)
		{
			ImGui::BulletText(operation->GetDescription());
		}
	}
}