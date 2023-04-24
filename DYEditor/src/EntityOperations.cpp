#include "Undo/Operations/EntityOperations.h"

#include "Serialization/SerializedObjectFactory.h"

namespace DYE::DYEditor
{
	EntityCreationOperation::EntityCreationOperation(World &world, Entity &entity) :
		m_pWorld(&world),
		m_EntityGUID(entity.GetComponent<IDComponent>().ID),
		m_SerializedEntity(SerializedObjectFactory::CreateSerializedEntity(entity))
	{
		sprintf(&m_Description[0], "Create new Entity (GUID: %s)", m_EntityGUID.ToString().c_str());
	}

	void EntityCreationOperation::Undo()
	{
		m_pWorld->DestroyEntityWithGUID(m_EntityGUID);
	}

	void EntityCreationOperation::Redo()
	{
		Entity entity = m_pWorld->CreateEntity();
		SerializedObjectFactory::ApplySerializedEntityToEmptyEntity(m_SerializedEntity, entity);
	}

	const char * EntityCreationOperation::GetDescription()
	{
		return &m_Description[0];
	}

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
		Entity entity = m_pWorld->CreateEntity();
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
}