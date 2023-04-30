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