#pragma once

#include <string>
#include <vector>
#include <memory>

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

	class GroupUndoOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return &Description[0];
		}

	public:
		char Description[128]{};
		std::vector<std::unique_ptr<UndoOperationBase>> OperationCollection;
	};
}