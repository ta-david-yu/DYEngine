#pragma once

#include "Serialization/SerializedComponent.h"
#include "Core/GUID.h"
#include "Type/TypeRegistry.h"
#include "Core/Scene.h"

namespace DYE::DYEditor
{
	class World;
	class Entity;

	class UndoOperationBase;
	class EntityDeletionOperation;

	class Undo
	{
	public:
		static void ClearAll();

		static bool HasOperationToUndo();
		static bool HasOperationToRedo();

		static void PerformUndo();
		static void PerformRedo();

		static void SetLatestOperationDescription(const char* fmt, ...);
		static void SetLatestOperationDescriptionArgs(const char* fmt, va_list args);

		/// All the undo operations called after StartGroupOperation & before EndGroupOperation
		/// will be collapsed into one group undoable operation.
		static void StartGroupOperation(const char *description);

		/// Set the description name of the current group operation,
		/// this method does nothing if the Undo system is not in a group operation.
		static void SetCurrentGroupOperationDescription(const char *description);

		/// See StartGroupOperation function for more details.
		static void EndGroupOperation();

		/// Call this after creating a new entity.
		static void RegisterEntityCreation(World& world, Entity entity);
		/// Call this after creating a new entity.
		static void RegisterEntityCreation(World& world, Entity entity, std::size_t indexInWorldHandleArray);

		/// Perform entity destruction (including all children) that can be restored with undo.
		static void DeleteEntityRecursively(Entity entity);
		/// Perform entity destruction (including all children) that can be restored with undo.
		static void DeleteEntityRecursively(Entity entity, std::size_t indexInWorldHandleArray);

		/// \param indexInParent the index of the parent's children list to insert to.
		/// By default it's -1, which means the last place in parent's hierarchy.
		static void SetEntityParent(Entity entity, int entityIndexBeforeSet, Entity newParent, int parentIndex, int indexInParent = -1);

		/// Move the entity to the top hierarchy (set parent to null basically) at the given index location.
		static void SetEntityOrderAtTopHierarchy(Entity entity, int entityIndexBeforeSet, int indexToInsert);

		// Call this after component modification.
		static void RegisterComponentModification(Entity entity,
												  SerializedComponent componentBeforeModification,
												  SerializedComponent componentAfterModification);

		// Perform component addition that can be restored with undo.
		static void AddComponent(Entity entity, std::string const &componentTypeName, ComponentTypeDescriptor typeDescriptor);

		// Perform component removal that can be restored with undo.
		static void RemoveComponent(Entity entity, const std::string &componentTypeName, ComponentTypeDescriptor typeDescriptor);

		// Perform system addition to a scene that can be restored with undo.
		static void AddSystem(Scene &scene, SystemDescriptor systemDescriptor, int orderInList);

		// Perform system removal from a scene that can be restored with undo.
		static void RemoveSystem(Scene &scene, SystemDescriptor systemDescriptor, int orderInList);

		// Toggle System.IsEnabled, can be restored with undo.
		static void SetSystemIsEnabled(Scene &scene, SystemDescriptor &systemDescriptor, int orderInList, bool value);

		// Reorder a system to a new location, the reorder can be restored with undo.
		static void ReorderSystem(Scene &scene, SystemDescriptor systemDescriptor, int oldOrderInList, int newOrderInList);

		static void DrawUndoHistoryWindow(bool *pIsOpen);

	private:
		/// This method only moves the given entity, children are ignored. For engine internal use.
		static void moveEntity(Entity entity, int indexBeforeMove, int indexToInsert);

		/// This method only deletes the entity. It does not update hierarchical information (i.e. children, parent).
		static EntityDeletionOperation *deleteEntityButNotChildren(Entity entity, std::size_t indexInWorldHandleArray);

		static void pushNewOperation(std::unique_ptr<UndoOperationBase> operation);
	};
}
