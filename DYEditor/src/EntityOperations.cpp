#include "Undo/Operations/EntityOperations.h"

#include "Serialization/SerializedObjectFactory.h"

namespace DYE::DYEditor
{
	// EntityCreationOperation

	EntityCreationOperation::EntityCreationOperation(World &world, Entity &entity) :
		m_pWorld(&world),
		m_EntityGUID(entity.GetComponent<IDComponent>().ID),
		m_SerializedEntity(SerializedObjectFactory::CreateSerializedEntity(entity))
	{
		sprintf(&m_Description[0], "Create New Entity (GUID: %s)", m_EntityGUID.ToString().c_str());
	}

	void EntityCreationOperation::Undo()
	{
		m_pWorld->DestroyEntityWithGUID(m_EntityGUID);
	}

	void EntityCreationOperation::Redo()
	{
		Entity entity = m_pWorld->CreateEntityAtIndex(m_IndexInWorldEntityArray);
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(m_SerializedEntity, entity);
	}

	const char * EntityCreationOperation::GetDescription()
	{
		return &m_Description[0];
	}

	// EntityDeletionOperation

	EntityDeletionOperation::EntityDeletionOperation(World& world, Entity& entityToDestroy) :
		m_pWorld(&world),
		m_EntityGUID(entityToDestroy.GetComponent<IDComponent>().ID),
		m_SerializedEntity(SerializedObjectFactory::CreateSerializedEntity(entityToDestroy))
	{
		sprintf(&m_Description[0], "Delete Entity '%s' (GUID: %s)",
				entityToDestroy.TryGetName().value().c_str(),
				m_EntityGUID.ToString().c_str());
		m_pWorld->DestroyEntityWithGUID(m_EntityGUID);
	}

	void EntityDeletionOperation::Undo()
	{
		Entity entity = m_pWorld->CreateEntityAtIndex(m_IndexInWorldEntityArray);
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(m_SerializedEntity, entity);
	}

	void EntityDeletionOperation::Redo()
	{
		m_pWorld->DestroyEntityWithGUID(m_EntityGUID);
	}

	const char * EntityDeletionOperation::GetDescription()
	{
		return &m_Description[0];
	}

	// ComponentModificationOperation

	ComponentModificationOperation::ComponentModificationOperation(Entity &entity,
																   SerializedComponent componentBeforeModification,
																   SerializedComponent componentAfterModification) :
																   m_pWorld(&entity.GetWorld()),
																   m_EntityGUID(entity.GetComponent<IDComponent>().ID),
																   m_SerializedComponentBeforeModification(componentBeforeModification),
																   m_SerializedComponentAfterModification(componentAfterModification)
	{
		auto componentTypeName = componentBeforeModification.TryGetTypeName().value();

		sprintf(&m_Description[0], "Modify Component '%s' in '%s' (GUID: %s)",
				componentTypeName.c_str(),
				entity.TryGetName().value().c_str(),
				m_EntityGUID.ToString().c_str());

		auto tryGetComponentTypeDescriptor = TypeRegistry::TryGetComponentTypeDescriptor(componentTypeName);
		DYE_ASSERT_LOG_WARN(tryGetComponentTypeDescriptor.has_value(),
							"Try to create a component modification operation but doesn't recognize the component type '%s'.", componentTypeName.c_str());

		m_ComponentTypeDescriptor = tryGetComponentTypeDescriptor.value();
	}

	void ComponentModificationOperation::Undo()
	{
		auto tryGetEntity = m_pWorld->TryGetEntityWithGUID(m_EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to undo a component modification operation but couldn't find the entity (GUID: %s).", m_EntityGUID.ToString().c_str());

		m_ComponentTypeDescriptor.Deserialize(m_SerializedComponentBeforeModification, tryGetEntity.value());
	}

	void ComponentModificationOperation::Redo()
	{
		auto tryGetEntity = m_pWorld->TryGetEntityWithGUID(m_EntityGUID);
		DYE_ASSERT_LOG_WARN(tryGetEntity.has_value(), "Try to redo a component modification operation but couldn't find the entity (GUID: %s).", m_EntityGUID.ToString().c_str());

		m_ComponentTypeDescriptor.Deserialize(m_SerializedComponentAfterModification, tryGetEntity.value());
	}

	const char *ComponentModificationOperation::GetDescription()
	{
		return &m_Description[0];
	}
}