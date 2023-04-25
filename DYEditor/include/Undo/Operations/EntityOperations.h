#pragma once

#include "Undo/UndoOperationBase.h"

#include "Type/TypeRegistry.h"
#include "Serialization/SerializedEntity.h"
#include "Core/World.h"
#include "Core/Entity.h"
#include "Components/IDComponent.h"

#include <string>

namespace DYE::DYEditor
{
	class EntityCreationOperation final : public UndoOperationBase
	{
		friend class Undo;

	public:
		EntityCreationOperation(World& world, Entity& entity);

		/// Destroy the newly created entity with the identifier.
		void Undo() override;

		/// Recreate an entity with the recorded GUID.
		void Redo() override;

		const char * GetDescription() override
		{
			return &m_Description[0];
		}

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		SerializedEntity m_SerializedEntity;
		int m_IndexInWorldEntityArray = 0;
	};

	class EntityDeletionOperation final : public UndoOperationBase
	{
		friend class Undo;
	public:
		EntityDeletionOperation(World& world, Entity& entityToDestroy);

		/// Recover the destroyed entity with the recorded GUID.
		void Undo() override;

		/// Destroy the entity with the
		void Redo() override;

		const char * GetDescription() override
		{
			return &m_Description[0];
		}

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		SerializedEntity m_SerializedEntity;
		std::size_t m_IndexInWorldEntityArray = 0;
	};

	class ComponentModificationOperation final : public UndoOperationBase
	{
	public:
		ComponentModificationOperation(Entity &entity,
									   SerializedComponent componentBeforeModification,
									   SerializedComponent componentAfterModification);

		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return &m_Description[0];
		}

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		SerializedComponent m_SerializedComponentBeforeModification;
		SerializedComponent m_SerializedComponentAfterModification;

		ComponentTypeDescriptor m_ComponentTypeDescriptor;
	};

	class ComponentAdditionOperation final : public UndoOperationBase
	{
	public:
		ComponentAdditionOperation(Entity &entity, std::string const &componentTypeName, ComponentTypeDescriptor typeDescriptor);

		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return &m_Description[0];
		}

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		ComponentTypeDescriptor m_ComponentTypeDescriptor;
	};

	class ComponentRemovalOperation final : public UndoOperationBase
	{
	public:
		ComponentRemovalOperation(Entity &entity,std::string const &typeName, ComponentTypeDescriptor typeDescriptor);

		void Undo() override;
		void Redo() override;

		const char *GetDescription() override
		{
			return &m_Description[0];
		}

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		ComponentTypeDescriptor m_ComponentTypeDescriptor;
		SerializedComponent m_SerializedComponentBeforeRemoval;
	};
}