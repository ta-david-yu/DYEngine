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

    class EntityMoveOperation final : public UndoOperationBase
    {
    public:
        void Undo() override;
        void Redo() override;

        World *pWorld;
        int IndexBeforeMove = 0;
        int IndexToInsert = 0;
    };

    class ComponentModificationOperation final : public UndoOperationBase
    {
    public:
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
        void Undo() override;
        void Redo() override;

        World *pWorld;
        GUID EntityGUID;
        std::string ComponentTypeName;
        ComponentTypeDescriptor TypeDescriptor;

        /// -1 means invalid
        int ComponentAdditionIndex = -1;
    };

    class ComponentRemovalOperation final : public UndoOperationBase
    {
    public:
        void Undo() override;
        void Redo() override;

        World *pWorld;
        GUID EntityGUID;
        std::string ComponentTypeName;
        ComponentTypeDescriptor TypeDescriptor;
        SerializedComponent SerializedComponentBeforeRemoval;

        /// -1 means invalid
        int ComponentOrderInListBeforeRemoval = -1;
    };
}