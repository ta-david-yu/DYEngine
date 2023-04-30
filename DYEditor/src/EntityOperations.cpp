#include "Undo/Operations/EntityOperations.h"

#include "Serialization/SerializedObjectFactory.h"

namespace DYE::DYEditor
{
	// EntityCreationOperation

	void EntityCreationOperation::Undo()
	{
		pWorld->DestroyEntityWithGUID(EntityGUID);
	}

	void EntityCreationOperation::Redo()
	{
		Entity entity = pWorld->createUntrackedEntity();
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(CreatedSerializedEntity, entity);
		pWorld->registerUntrackedEntityAtIndex(entity, IndexInWorldEntityArray);
	}

	// EntityDeletionOperation

	void EntityDeletionOperation::Undo()
	{
		Entity entity = pWorld->createUntrackedEntity();
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(DeletedSerializedEntity, entity);
		pWorld->registerUntrackedEntityAtIndex(entity, IndexInWorldEntityArray);
	}

	void EntityDeletionOperation::Redo()
	{
		pWorld->DestroyEntityWithGUID(EntityGUID);
	}

	// EntityMoveOperation

	void EntityMoveOperation::Undo()
	{
		auto &entityHandles = pWorld->m_EntityHandles;
		if (IndexBeforeMove < IndexToInsert)
		{
			auto handle = entityHandles[IndexToInsert - 1];
			entityHandles.insert(entityHandles.begin() + IndexBeforeMove, handle);
			entityHandles.erase(entityHandles.begin() + IndexToInsert);
		}
		else if (IndexBeforeMove > IndexToInsert)
		{
			auto handle = entityHandles[IndexToInsert];
			entityHandles.insert(entityHandles.begin() + IndexBeforeMove + 1, handle);
			entityHandles.erase(entityHandles.begin() + IndexToInsert);
		}
	}

	void EntityMoveOperation::Redo()
	{
		auto &entityHandles = pWorld->m_EntityHandles;
		if (IndexBeforeMove < IndexToInsert)
		{
			entityHandles.insert(entityHandles.begin() + IndexToInsert, entityHandles[IndexBeforeMove]);

			// The old index is smaller than the new index.
			// We can safely erase it with the original index.
			entityHandles.erase(entityHandles.begin() + IndexBeforeMove);
		}
		else if (IndexBeforeMove > IndexToInsert)
		{
			entityHandles.insert(entityHandles.begin() + IndexToInsert, entityHandles[IndexBeforeMove]);

			// The old index is bigger than the new index.
			// We need to increment the erasure index by 1.
			entityHandles.erase(entityHandles.begin() + IndexBeforeMove + 1);
		}
	}

	// ComponentModificationOperation

	void ComponentModificationOperation::Undo()
	{
		auto tryGetEntity = pWorld->TryGetEntityWithGUID(EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to undo a component modification operation but couldn't find the entity (GUID: %s).", EntityGUID.ToString().c_str());

		TypeDescriptor.Deserialize(SerializedComponentBeforeModification, tryGetEntity.value());
	}

	void ComponentModificationOperation::Redo()
	{
		auto tryGetEntity = pWorld->TryGetEntityWithGUID(EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to redo a component modification operation but couldn't find the entity (GUID: %s).", EntityGUID.ToString().c_str());

		TypeDescriptor.Deserialize(SerializedComponentAfterModification, tryGetEntity.value());
	}

	// ComponentAdditionOperation

	void ComponentAdditionOperation::Undo()
	{
		auto tryGetEntity = pWorld->TryGetEntityWithGUID(EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to undo a component addition operation but couldn't find the entity (GUID: %s).", EntityGUID.ToString().c_str());

		TypeDescriptor.Remove(tryGetEntity.value());
	}

	void ComponentAdditionOperation::Redo()
	{
		auto tryGetEntity = pWorld->TryGetEntityWithGUID(EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to redo a component addition operation but couldn't find the entity (GUID: %s).", EntityGUID.ToString().c_str());

		TypeDescriptor.Add(tryGetEntity.value());
	}

	// ComponentRemovalOperation

	void ComponentRemovalOperation::Undo()
	{
		auto tryGetEntity = pWorld->TryGetEntityWithGUID(EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to undo a component removal operation but couldn't find the entity (GUID: %s).", EntityGUID.ToString().c_str());

		TypeDescriptor.Deserialize(SerializedComponentBeforeRemoval, tryGetEntity.value());
	}

	void ComponentRemovalOperation::Redo()
	{
		auto tryGetEntity = pWorld->TryGetEntityWithGUID(EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to redo a component removal operation but couldn't find the entity (GUID: %s).", EntityGUID.ToString().c_str());

		TypeDescriptor.Remove(tryGetEntity.value());
	}
}