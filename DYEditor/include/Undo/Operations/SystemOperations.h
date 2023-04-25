#pragma once

#include "Undo/UndoOperationBase.h"

#include "Type/TypeRegistry.h"
#include "Serialization/SerializedEntity.h"
#include "Core/Scene.h"
#include "Core/World.h"
#include "Core/EditorSystem.h"

namespace DYE::DYEditor
{
	class SystemAdditionOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return Description;
		}

	public:
		char Description[128]{};
		SystemDescriptor Descriptor;
		Scene* pScene = nullptr;
		ExecutionPhase ExecutionPhase = ExecutionPhase::Initialize;
	};

	class SystemRemovalOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return Description;
		}

	public:
		char Description[128]{};
		std::string SystemTypeName;
		Scene* pScene = nullptr;
		ExecutionPhase SystemExecutionPhase;
	};

	class ToggleSystemOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return Description;
		}

	public:
		char Description[128]{};
		std::string SystemTypeName;
		Scene* pScene = nullptr;
		ExecutionPhase SystemExecutionPhase;
		bool IsEnabled = true;
	};
}