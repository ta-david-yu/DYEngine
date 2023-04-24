#pragma once

#include <string>

namespace DYE::DYEditor
{
	class UndoOperationBase
	{
	public:
		virtual void Undo() = 0;
		virtual void Redo() = 0;
		virtual const char * GetDescription() = 0;

		virtual ~UndoOperationBase() = default;
	};
}