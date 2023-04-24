#pragma once

#include "Serialization/SerializedComponent.h"
#include "Core/GUID.h"

namespace DYE::DYEditor
{
	class World;
	class Entity;

	class Undo
	{
	public:
		static void ClearAll();

		static bool HasOperationToUndo();
		static bool HasOperationToRedo();

		static void PerformUndo();
		static void PerformRedo();

		// Call this after component modification.
		static void RegisterComponentModification(World& world,
												  Entity& entity,
												  SerializedComponent componentBeforeModification,
												  SerializedComponent componentAfterModification);

		// Call this after creating a new entity.
		static void RegisterEntityCreation(World& world, Entity& entity);

		// Destroy an entity that can be restored with undo.
		static void DeleteEntity(World& world, Entity& entity);

		static void DrawUndoHistoryWindow(bool *pIsOpen);
	};
}
