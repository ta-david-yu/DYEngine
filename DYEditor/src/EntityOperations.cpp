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
		Entity entity = pWorld->CreateEntityAtIndex(IndexInWorldEntityArray);
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(CreatedSerializedEntity, entity);
	}

	// EntityDeletionOperation

	void EntityDeletionOperation::Undo()
	{
		Entity entity = pWorld->CreateEntityAtIndex(IndexInWorldEntityArray);
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(DeletedSerializedEntity, entity);
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

	ComponentAdditionOperation::ComponentAdditionOperation(Entity &entity, std::string const &componentTypeName, ComponentTypeDescriptor typeDescriptor) :
		pWorld(&entity.GetWorld()),
		EntityGUID(entity.GetComponent<IDComponent>().ID),
		TypeDescriptor(typeDescriptor)
	{
		sprintf(&Description[0], "Add %s to Entity '%s' (GUID: %s)",
				componentTypeName.c_str(),
				entity.TryGetName().value().c_str(),
				EntityGUID.ToString().c_str());

		TypeDescriptor.Add(entity);
	}

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

	ComponentRemovalOperation::ComponentRemovalOperation(Entity &entity, std::string const &typeName, ComponentTypeDescriptor typeDescriptor) :
		pWorld(&entity.GetWorld()),
		EntityGUID(entity.GetComponent<IDComponent>().ID),
		TypeDescriptor(typeDescriptor),
		SerializedComponentBeforeRemoval(SerializedObjectFactory::CreateSerializedComponentOfType(entity, typeName, typeDescriptor))
	{
		sprintf(&Description[0], "Remove %s from Entity '%s' (GUID: %s)",
				typeName.c_str(),
				entity.TryGetName().value().c_str(),
				EntityGUID.ToString().c_str());

		TypeDescriptor.Remove(entity);
	}

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