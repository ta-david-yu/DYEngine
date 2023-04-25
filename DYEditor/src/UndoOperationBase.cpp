#include "Undo/UndoOperationBase.h"

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
}