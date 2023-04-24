#pragma once

#include "Serialization/SerializedComponent.h"
#include "Core/GUID.h"

namespace DYE::DYEditor
{
	class World;
	class Entity;

	class UndoOperationBase;

	class Undo
	{
	public:
		static void ClearAll();

		static bool HasOperationToUndo();
		static bool HasOperationToRedo();

		static void PerformUndo();
		static void PerformRedo();

		// Call this after creating a new entity.
		static void RegisterEntityCreation(World& world, Entity& entity);
		// Call this after creating a new entity.
		static void RegisterEntityCreation(World& world, Entity& entity, std::size_t indexInWorldHandleArray);

		// Destroy an entity that can be restored with undo.
		static void DeleteEntity(World& world, Entity& entity);
		// Destroy an entity that can be restored with undo.
		static void DeleteEntity(World& world, Entity& entity, std::size_t indexInWorldHandleArray);

		// Call this after component modification.
		static void RegisterComponentModification(Entity& entity,
												  SerializedComponent componentBeforeModification,
												  SerializedComponent componentAfterModification);

		static void DrawUndoHistoryWindow(bool *pIsOpen);

	private:
		static void pushNewOperation(std::unique_ptr<UndoOperationBase> operation);
	};
}
