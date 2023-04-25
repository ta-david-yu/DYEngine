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
	public:
		/// Destroy the newly created entity with the identifier.
		void Undo() override;

		/// Recreate an entity with the recorded GUID.
		void Redo() override;

		World *pWorld;
		GUID EntityGUID;
		SerializedEntity CreatedSerializedEntity;
		int IndexInWorldEntityArray = 0;
	};

	class EntityDeletionOperation final : public UndoOperationBase
	{
	public:
		/// Recover the destroyed entity with the recorded GUID.
		void Undo() override;

		/// Destroy the entity with the
		void Redo() override;

		World *pWorld;
		GUID EntityGUID;
		SerializedEntity DeletedSerializedEntity;
		std::size_t IndexInWorldEntityArray = 0;
	};

	class ComponentModificationOperation final : public UndoOperationBase
	{
	public:
		ComponentModificationOperation(Entity &entity,
									   SerializedComponent componentBeforeModification,
									   SerializedComponent componentAfterModification);

		void Undo() override;
		void Redo() override;

		World *pWorld;
		GUID EntityGUID;
		SerializedComponent SerializedComponentBeforeModification;
		SerializedComponent SerializedComponentAfterModification;

		ComponentTypeDescriptor TypeDescriptor;
	};

	class ComponentAdditionOperation final : public UndoOperationBase
	{
	public:
		ComponentAdditionOperation(Entity &entity, std::string const &componentTypeName, ComponentTypeDescriptor typeDescriptor);

		void Undo() override;
		void Redo() override;

		World *pWorld;
		GUID EntityGUID;
		ComponentTypeDescriptor TypeDescriptor;
	};

	class ComponentRemovalOperation final : public UndoOperationBase
	{
	public:
		ComponentRemovalOperation(Entity &entity,std::string const &typeName, ComponentTypeDescriptor typeDescriptor);

		void Undo() override;
		void Redo() override;

		World *pWorld;
		GUID EntityGUID;
		ComponentTypeDescriptor TypeDescriptor;
		SerializedComponent SerializedComponentBeforeRemoval;
	};
}