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

		SystemDescriptor Descriptor;
		Scene* pScene = nullptr;
		ExecutionPhase ExecutionPhase = ExecutionPhase::Initialize;
		int OrderInSystemList = 0;
	};

	class SystemRemovalOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		SystemDescriptor Descriptor;
		Scene* pScene = nullptr;
		ExecutionPhase ExecutionPhase = ExecutionPhase::Initialize;
		int OrderInSystemList = 0;
	};

	class ToggleSystemOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		std::string SystemTypeName;
		Scene* pScene = nullptr;
		ExecutionPhase ExecutionPhase;
		bool IsEnabled = true;
	};

	class SystemReorderOperation final : public UndoOperationBase
	{
	public:
		void Undo() override;
		void Redo() override;

		Scene* pScene = nullptr;
		SystemBase* pSystemBase = nullptr;
		int OrderBeforeModification = 0;
		int OrderAfterModification = 0;
	};
}