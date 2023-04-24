#pragma once

#include "Undo/UndoOperationBase.h"

#include "Serialization/SerializedEntity.h"
#include "Core/Entity.h"
#include "Core/World.h"
#include "Components/IDComponent.h"

#include <string>

namespace DYE::DYEditor
{
	class EntityCreationOperation final : public UndoOperationBase
	{
	public:
		EntityCreationOperation(World& world, Entity& entity);

		/// Destroy the newly created entity with the identifier.
		void Undo() override;

		/// Recreate an entity with the recorded GUID.
		void Redo() override;

		const char * GetDescription() override;

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		SerializedEntity m_SerializedEntity;
	};

	class EntityDeletionOperation final : public UndoOperationBase
	{
	public:
		EntityDeletionOperation(World& world, Entity& entityToDestroy);

		/// Recover the destroyed entity with the recorded GUID.
		void Undo() override;

		/// Destroy the entity with the
		void Redo() override;

		const char * GetDescription() override;

	private:
		char m_Description[128]{};

		World *m_pWorld;
		GUID m_EntityGUID;
		SerializedEntity m_SerializedEntity;
	};
}