#include "Undo/Undo.h"

#include "Undo/UndoOperationBase.h"
#include "Undo/Operations/EntityOperations.h"
#include "Undo/Operations/SystemOperations.h"

#include "Serialization/SerializedObjectFactory.h"
#include "Type/BuiltInTypeRegister.h"
#include "Util/EntityUtil.h"
#include "ImGui/ImGuiUtil.h"

#include <memory>
#include <vector>

namespace DYE::DYEditor
{
	struct UndoData
	{
		std::vector<std::unique_ptr<UndoOperationBase>> Operations;
		int LatestOperationIndex = -1;

		bool IsInGroup = false;
		int CurrentGroupBeginIndex = -1;
		std::unique_ptr<GroupUndoOperation> CurrentGroupOperation;
	};

	static UndoData s_Data;

	void Undo::ClearAll()
	{
		s_Data.Operations.clear();
		s_Data.LatestOperationIndex = -1;
	}

	bool Undo::HasOperationToUndo()
	{
		return s_Data.LatestOperationIndex >= 0;
	}

	bool Undo::HasOperationToRedo()
	{
		return s_Data.LatestOperationIndex + 1 < s_Data.Operations.size();
	}

	void Undo::PerformUndo()
	{
		if (s_Data.LatestOperationIndex < 0)
		{
			// No operation to be undone.
			return;
		}

		s_Data.Operations[s_Data.LatestOperationIndex]->Undo();
		s_Data.LatestOperationIndex--;
	}

	void Undo::PerformRedo()
	{
		int nextOperationIndexToRedo = s_Data.LatestOperationIndex + 1;
		if (nextOperationIndexToRedo >= s_Data.Operations.size())
		{
			// No operation to be redone.
			return;
		}

		s_Data.LatestOperationIndex = nextOperationIndexToRedo;
		s_Data.Operations[s_Data.LatestOperationIndex]->Redo();
	}

	void Undo::SetLatestOperationDescription(const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		SetLatestOperationDescriptionArgs(fmt, args);
		va_end(args);
	}

	void Undo::SetLatestOperationDescriptionArgs(const char *fmt, va_list args)
	{
		if (s_Data.LatestOperationIndex < 0 || s_Data.LatestOperationIndex >= s_Data.Operations.size())
		{
			// Invalid latest operation.
			return;
		}

#if defined(_MSC_VER)
		int offset = vsprintf_s(s_Data.Operations[s_Data.LatestOperationIndex]->Description, fmt, args);
#else
		int offset = std::vsprintf(s_Data.Operations[s_Data.LatestOperationIndex]->Description, fmt, args);
#endif
	}

	void Undo::StartGroupOperation(const char *description)
	{
		if (s_Data.IsInGroup)
		{
			// It's already in a group operation.
			// End the previous one first.
			EndGroupOperation();
		}

		s_Data.IsInGroup = true;
		s_Data.CurrentGroupBeginIndex = s_Data.LatestOperationIndex + 1;
		s_Data.CurrentGroupOperation = std::make_unique<GroupUndoOperation>();

#if defined(_MSC_VER)
		strcpy_s(s_Data.CurrentGroupOperation->Description, description);
#else
		std::strcpy(s_Data.CurrentGroupOperation->Description, description);
#endif
	}

	void Undo::SetCurrentGroupOperationDescription(const char *description)
	{
		if (!s_Data.IsInGroup)
		{
			return;
		}

#if defined(_MSC_VER)
		strcpy_s(s_Data.CurrentGroupOperation->Description, description);
#else
		std::strcpy(s_Data.CurrentGroupOperation->Description, description);
#endif
	}

	void Undo::EndGroupOperation()
	{
		DYE_ASSERT_LOG_WARN(s_Data.IsInGroup, "You shouldn't call EndGroupOperation without calling StartGroupOperation first.");

		s_Data.IsInGroup = false;

		bool const isEmptyGroupOperation = s_Data.LatestOperationIndex < s_Data.CurrentGroupBeginIndex;
		if (isEmptyGroupOperation)
		{
			DYE_LOG("Ending Undo Group Operation '%s' without any actual operation inside.", s_Data.CurrentGroupOperation->Description);
			auto ptr = s_Data.CurrentGroupOperation.release();
			delete ptr; // Because we release ownership of the operation manually, we need to free the memory on our own.
			return;
		}

		// Collapse operations into the group & remove it from the undo list.
		int const numberCollapsedOfOperations = (s_Data.LatestOperationIndex + 1) - s_Data.CurrentGroupBeginIndex;
		auto rangeToInsertBegin = s_Data.Operations.begin() + s_Data.CurrentGroupBeginIndex;
		auto rangeToInsertEnd = s_Data.Operations.begin() + s_Data.LatestOperationIndex + 1;

		s_Data.CurrentGroupOperation->OperationCollection.insert(
										s_Data.CurrentGroupOperation->OperationCollection.end(),
										std::make_move_iterator(rangeToInsertBegin),
										std::make_move_iterator(rangeToInsertEnd));

		s_Data.Operations.erase(rangeToInsertBegin, rangeToInsertEnd);

		sprintf(s_Data.CurrentGroupOperation->Description, "%s (Group with %d Operations)", s_Data.CurrentGroupOperation->Description, numberCollapsedOfOperations);
		s_Data.Operations.emplace_back(std::move(s_Data.CurrentGroupOperation));
		s_Data.LatestOperationIndex = s_Data.Operations.size() - 1;
	}

	void Undo::RegisterEntityCreation(World &world, Entity entity)
	{
		auto tryGetIndexResult = world.TryGetEntityIndex(entity);
		RegisterEntityCreation(world, entity, tryGetIndexResult.has_value()? tryGetIndexResult.value() : 0);
	}

	void Undo::RegisterEntityCreation(World &world, Entity entity, std::size_t indexInWorldHandleArray)
	{
		auto operation = std::make_unique<EntityCreationOperation>();
		operation->pWorld = &world;
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->CreatedSerializedEntity = SerializedObjectFactory::CreateSerializedEntity(entity);
		operation->IndexInWorldEntityArray = indexInWorldHandleArray;

		sprintf(operation->Description, "Create New Entity (GUID: %s)", operation->EntityGUID.ToString().c_str());

		pushNewOperation(std::move(operation));
	}

	void Undo::DeleteEntityRecursively(Entity entity)
	{
		auto tryGetIndexResult = entity.GetWorld().TryGetEntityIndex(entity);
		DYE_ASSERT_LOG_WARN(tryGetIndexResult.has_value(), "Couldn't find the given entity inside the handle array.");

		DeleteEntityRecursively(entity, tryGetIndexResult.has_value() ? tryGetIndexResult.value() : 0);
	}

	void Undo::DeleteEntityRecursively(Entity entity, std::size_t indexInWorldHandleArray)
	{
		Undo::StartGroupOperation("Delete Entity Recursively (On-going)");

		// Get name, guid and the children of the entity before deleting it.
		std::string entityName = entity.TryGetName().value();
		GUID entityGUID = entity.TryGetGUID().value();
		auto allChildren = EntityUtil::GetAllChildrenPreorder(entity);

		// Remove the entity from its old Parent's ChildrenComponent if it has a parent.
		auto tryGetOldParent = entity.TryGetComponent<ParentComponent>();
		if (tryGetOldParent.has_value())
		{
			auto tryGetOldParentEntity = entity.GetWorld().TryGetEntityWithGUID(tryGetOldParent.value().get().ParentGUID);
			DYE_ASSERT_LOG_WARN(tryGetOldParentEntity.has_value(),
								"The entity has a parent component already but the old GUID '%s' wasn't referencing to a valid entity.",
								tryGetOldParent.value().get().ParentGUID.ToString().c_str());

			Entity oldParent = tryGetOldParentEntity.value();

			auto tryGetChildren = oldParent.TryGetComponent<ChildrenComponent>();
			ChildrenComponent *pChildrenComponent = nullptr;
			if (tryGetChildren.has_value())
			{
				pChildrenComponent = &tryGetChildren.value().get();
			}
			else
			{
				// If the parent doesn't have a children component,
				// we will add one first and then do the modification.
				Undo::AddComponent(oldParent, ChildrenComponentName,
								   TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
				pChildrenComponent = &oldParent.GetComponent<ChildrenComponent>();
			}

			auto serializedChildrenComponentBeforeModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(oldParent, ChildrenComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
			// Erase the entity from the old parent's children list.
			std::erase(pChildrenComponent->ChildrenGUIDs, entityGUID);
			auto serializedChildrenComponentAfterModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(oldParent, ChildrenComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());

			Undo::RegisterComponentModification(oldParent, serializedChildrenComponentBeforeModification,
												serializedChildrenComponentAfterModification);
		}

		// Delete the entity and its children.
		EntityDeletionOperation *pParentDeletionOperation = deleteEntityButNotChildren(entity, indexInWorldHandleArray);
		for (auto childEntity : allChildren)
		{
			deleteEntityButNotChildren(childEntity, indexInWorldHandleArray);
		}

		// Set the operation description based on the result.
		char operationDescription[128] = "";
		sprintf(operationDescription,
				"Delete Entity '%s' and %zu children",
				entityName.c_str(),
				allChildren.size());
		Undo::SetCurrentGroupOperationDescription(operationDescription);

		Undo::EndGroupOperation();
	}

	EntityDeletionOperation *Undo::deleteEntityButNotChildren(Entity entity, std::size_t indexInWorldHandleArray)
	{
		auto operation = std::make_unique<EntityDeletionOperation>();
		operation->pWorld = &entity.GetWorld();
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->DeletedSerializedEntity = SerializedObjectFactory::CreateSerializedEntity(entity);
		operation->IndexInWorldEntityArray = indexInWorldHandleArray;

		sprintf(operation->Description, "Delete Entity '%s' (GUID: %s)", entity.TryGetName().value().c_str(), operation->EntityGUID.ToString().c_str());

		operation->pWorld->destroyEntityByGUIDButNotChildren(operation->EntityGUID);

		EntityDeletionOperation *rawOperationPtr = operation.get();
		pushNewOperation(std::move(operation));

		return rawOperationPtr;
	}

	void Undo::moveEntity(Entity entity, int indexBeforeMove, int indexToInsert)
	{
		auto operation = std::make_unique<EntityMoveOperation>();
		operation->pWorld = &entity.GetWorld();
		operation->IndexBeforeMove = indexBeforeMove;
		operation->IndexToInsert = indexToInsert;

		auto &entityHandles = operation->pWorld->m_EntityHandles;
		if (indexBeforeMove < indexToInsert)
		{
			entityHandles.insert(entityHandles.begin() + indexToInsert, entityHandles[indexBeforeMove]);

			// The old index is smaller than the new index.
			// We can safely erase it with the original index.
			entityHandles.erase(entityHandles.begin() + indexBeforeMove);
		}
		else if (indexBeforeMove > indexToInsert)
		{
			entityHandles.insert(entityHandles.begin() + indexToInsert, entityHandles[indexBeforeMove]);

			// The old index is bigger than the new index.
			// We need to increment the erasure index by 1.
			entityHandles.erase(entityHandles.begin() + indexBeforeMove + 1);
		}

		sprintf(operation->Description, "Move Entity '%s' from %d to %d",
				entity.TryGetName().value().c_str(),
				indexBeforeMove,
				indexToInsert);

		pushNewOperation(std::move(operation));
	}

	void Undo::SetEntityParent(Entity entity, int entityIndexBeforeSet, Entity newParent, int parentIndex, int indexInParent)
	{
		if (entity == newParent)
		{
			DYE_LOG("You cannot set an entity as its own parent. SetEntityParent operation skipped.");
			return;
		}

		auto tryGetEntityGUID = entity.TryGetGUID();
		if (!tryGetEntityGUID.has_value())
		{
			DYE_LOG("The given entity doesn't have a GUID. SetEntityParent operation skipped.");
			return;
		}

		auto tryGetParentGUID = newParent.TryGetGUID();
		if (!tryGetParentGUID.has_value())
		{
			DYE_LOG("The given parent doesn't have a GUID. SetEntityParent operation skipped.");
			return;
		}

		if (EntityUtil::IsChildOf(newParent, entity))
		{
			DYE_LOG("You cannot make an entity be the child of its own child.");
			return;
		}

		auto entityGUID = tryGetEntityGUID.value();
		auto parentGUID = tryGetParentGUID.value();

		// This is a group operation that includes:
		//	1. A collection of MoveEntity operations: to move root, children, children of children... under the new parent.
		//	2. Three ComponentModification operations: to modify entity's original parent.ChildrenComponent & entity.ParentComponent & parent.ChildrenComponent.
		char operationDescription[128] = "";
		sprintf(operationDescription, "Set the parent of '%s' to '%s'", entity.TryGetName().value().c_str(), newParent.TryGetName().value().c_str());
		Undo::StartGroupOperation(operationDescription);

		// Move entities order in the entity handle array.
		// We want to insert at parent index + 1 because that's the head of the parent's children list (if there is any).
		// TODO A: update the index to the end of the parent children list so we could put the entity at the end of the children list
		// 	See below TODO B.

		// AddChildrenComponent: try to access parent's children component OR add a new one.
		auto tryGetParentEntityChildren = newParent.TryGetComponent<ChildrenComponent>();
		ChildrenComponent *pNewParentEntityChildrenComponent = nullptr;
		if (tryGetParentEntityChildren.has_value())
		{
			pNewParentEntityChildrenComponent = &tryGetParentEntityChildren.value().get();
		}
		else
		{
			// If the parent doesn't have a children component,
			// we will add one first and then do the modification.
			Undo::AddComponent(newParent, ChildrenComponentName,
							   TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
			pNewParentEntityChildrenComponent = &newParent.GetComponent<ChildrenComponent>();
		}

		if (indexInParent >= pNewParentEntityChildrenComponent->ChildrenGUIDs.size())
		{
			// If the index in parent is bigger than parent's children list size,
			// We will just assume the user wants to insert it at the last place.
			indexInParent = -1;
		}

		std::vector<Entity> newParentAndItsChildren = EntityUtil::GetEntityAndAllChildrenPreorder(newParent);

		// When the offset is -1, we want to place it at the end of the parent hierarchy.
		bool const insertAtTheLastLocationInParentHierarchy = indexInParent == -1;

		// Calculate the actual index offset as if the hierarchy is completely flat in pre-order.
		int flatInsertIndexOffset = 0;
		if (insertAtTheLastLocationInParentHierarchy)
		{
			flatInsertIndexOffset = newParentAndItsChildren.size();
		}
		else if (indexInParent == 0)
		{
			flatInsertIndexOffset = 1;
		}
		else
		{
			// We start from 1 because 0 is the new parent.
			for (flatInsertIndexOffset = 1; flatInsertIndexOffset < newParentAndItsChildren.size(); flatInsertIndexOffset++)
			{
				Entity child = newParentAndItsChildren[flatInsertIndexOffset];
				if (child.TryGetGUID().value() == pNewParentEntityChildrenComponent->ChildrenGUIDs[indexInParent])
				{
					// We find the flat index, break.
					break;
				}
			}
		}

		int indexToInsert = parentIndex + flatInsertIndexOffset;
		int movePointer = entityIndexBeforeSet;

		if (entityIndexBeforeSet < indexToInsert)
		{
			// Moving under an entity that's behind in the entity handle list,
			// we don't need to increment our pointer/index.
			EntityUtil::ForEntityAndEachChildPreorder
			(
				entity,
				[&indexToInsert, &movePointer](Entity childEntity)
				{
					Undo::moveEntity(childEntity, movePointer, indexToInsert);
				}
			);
		}
		else
		{
			EntityUtil::ForEntityAndEachChildPreorder
			(
				entity,
				[&indexToInsert, &movePointer](Entity childEntity)
				{
					Undo::moveEntity(childEntity, movePointer, indexToInsert);
					movePointer++;
					indexToInsert++;
				}
			);
		}

		// Entity's old Parent's ChildrenComponent modification.
		auto tryGetOldParent = entity.TryGetComponent<ParentComponent>();
		if (tryGetOldParent.has_value())
		{
			auto tryGetOldParentEntity = entity.GetWorld().TryGetEntityWithGUID(tryGetOldParent.value().get().ParentGUID);
			DYE_ASSERT_LOG_WARN(tryGetOldParentEntity.has_value(),
								"The entity has a parent component already but the old GUID '%s' wasn't referencing to a valid entity.",
								tryGetOldParent.value().get().ParentGUID.ToString().c_str());

			Entity oldParent = tryGetOldParentEntity.value();

			auto tryGetChildren = oldParent.TryGetComponent<ChildrenComponent>();
			ChildrenComponent *pChildrenComponent = nullptr;
			if (tryGetChildren.has_value())
			{
				pChildrenComponent = &tryGetChildren.value().get();
			}
			else
			{
				// If the parent doesn't have a children component,
				// we will add one first and then do the modification.
				Undo::AddComponent(oldParent, ChildrenComponentName,
								   TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
				pChildrenComponent = &oldParent.GetComponent<ChildrenComponent>();
			}

			auto serializedChildrenComponentBeforeModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(oldParent, ChildrenComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
			// Remove the entity from the old parent's children list.
			std::erase(pChildrenComponent->ChildrenGUIDs, entityGUID);
			auto serializedChildrenComponentAfterModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(oldParent, ChildrenComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());

			Undo::RegisterComponentModification(oldParent, serializedChildrenComponentBeforeModification,
												serializedChildrenComponentAfterModification);

			if (newParent != oldParent)
			{
				// If the children list is empty after the child removal & the new parent is not the old parent,
				// we also remove the children component from the old parent entity.
				if (pChildrenComponent->ChildrenGUIDs.empty())
				{
					Undo::RemoveComponent(oldParent, ChildrenComponentName, TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
				}
			}
		}

		// Entity's ParentComponent modification.
		{
			auto tryGetEntityParent = entity.TryGetComponent<ParentComponent>();
			ParentComponent *pParentComponent = nullptr;
			if (tryGetEntityParent.has_value())
			{
				pParentComponent = &tryGetEntityParent.value().get();
			}
			else
			{
				// If the entity doesn't have a parent component,
				// we will add one first and then do the modification.
				Undo::AddComponent(entity, ParentComponentName,
								   TypeRegistry::GetComponentTypeDescriptor_ParentComponent());
				pParentComponent = &entity.GetComponent<ParentComponent>();
			}

			auto serializedParentComponentBeforeModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(entity, ParentComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ParentComponent());
			pParentComponent->ParentGUID = parentGUID;
			auto serializedParentComponentAfterModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(entity, ParentComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ParentComponent());

			Undo::RegisterComponentModification(entity, serializedParentComponentBeforeModification,
												serializedParentComponentAfterModification);
		}

		// ParentEntity's ChildrenComponent modification.
		{
			auto serializedChildrenComponentBeforeModification =
				SerializedObjectFactory::CreateSerializedComponentOfType
				(
					newParent,
					ChildrenComponentName,
					TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent()
				);

			auto &newParentChildren = newParent.GetComponent<ChildrenComponent>();
			if (insertAtTheLastLocationInParentHierarchy)
			{
				newParentChildren.ChildrenGUIDs.insert(newParentChildren.ChildrenGUIDs.end(), entityGUID);
			}
			else
			{
				newParentChildren.ChildrenGUIDs.insert(newParentChildren.ChildrenGUIDs.begin() + indexInParent, entityGUID);
			}
			auto serializedChildrenComponentAfterModification =
				SerializedObjectFactory::CreateSerializedComponentOfType
				(
					newParent,
					ChildrenComponentName,
					TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent()
				);

			Undo::RegisterComponentModification(newParent, serializedChildrenComponentBeforeModification,
												serializedChildrenComponentAfterModification);
		}

		Undo::EndGroupOperation();
	}

	void Undo::SetEntityOrderAtTopHierarchy(Entity entity, int entityIndexBeforeSet, int indexToInsert)
	{
		auto tryGetEntityGUID = entity.TryGetGUID();
		if (!tryGetEntityGUID.has_value())
		{
			DYE_LOG("The given entity doesn't have a GUID. SetEntityParent operation skipped.");
			return;
		}

		auto entityGUID = tryGetEntityGUID.value();

		char operationDescription[128] = "";
		sprintf(operationDescription, "Move Entity '%s' at Top Hierarchy", entity.TryGetName().value().c_str());
		Undo::StartGroupOperation(operationDescription);

		int movePointer = entityIndexBeforeSet;
		if (entityIndexBeforeSet < indexToInsert)
		{
			// Moving under an entity that's behind in the entity handle list,
			// we don't need to increment our pointer/index.
			EntityUtil::ForEntityAndEachChildPreorder
			(
				entity,
				[&indexToInsert, &movePointer](Entity childEntity)
				{
					Undo::moveEntity(childEntity, movePointer, indexToInsert);
				}
			);
		}
		else
		{
			EntityUtil::ForEntityAndEachChildPreorder
			(
				entity,
				[&indexToInsert, &movePointer](Entity childEntity)
				{
					Undo::moveEntity(childEntity, movePointer, indexToInsert);
					movePointer++;
					indexToInsert++;
				}
			);
		}

		// Try to remove the entity from the old parent's children list & remove the entity's parent component if there is a parent.
		auto tryGetOldParent = entity.TryGetComponent<ParentComponent>();
		if (tryGetOldParent.has_value())
		{
			auto tryGetOldParentEntity = entity.GetWorld().TryGetEntityWithGUID(
				tryGetOldParent.value().get().ParentGUID);
			DYE_ASSERT_LOG_WARN(tryGetOldParentEntity.has_value(),
								"The entity has a parent component already but the old GUID '%s' wasn't referencing to a valid entity.",
								tryGetOldParent.value().get().ParentGUID.ToString().c_str());

			Entity oldParent = tryGetOldParentEntity.value();

			auto tryGetChildren = oldParent.TryGetComponent<ChildrenComponent>();
			DYE_ASSERT_LOG_WARN(tryGetChildren.has_value(),
								"The old parent should have children component but it doesn't for some reasons. "
								"The entity hierarchy state could be defected.");

			ChildrenComponent &childrenComponent = tryGetChildren.value().get();

			auto serializedChildrenComponentBeforeModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(oldParent, ChildrenComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
			// Remove the entity from the old parent's children list.
			std::erase(childrenComponent.ChildrenGUIDs, entityGUID);
			auto serializedChildrenComponentAfterModification =
				SerializedObjectFactory::CreateSerializedComponentOfType(oldParent, ChildrenComponentName,
																		 TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());

			Undo::RegisterComponentModification(oldParent, serializedChildrenComponentBeforeModification,
												serializedChildrenComponentAfterModification);

			// If the children list is empty after the child removal,
			// we also remove the children component from the old parent entity.
			if (childrenComponent.ChildrenGUIDs.empty())
			{
				Undo::RemoveComponent(oldParent, ChildrenComponentName, TypeRegistry::GetComponentTypeDescriptor_ChildrenComponent());
			}

			// Remove the parent component from the entity.
			Undo::RemoveComponent(entity, ParentComponentName, TypeRegistry::GetComponentTypeDescriptor_ParentComponent());
		}

		Undo::EndGroupOperation();
	}

	void Undo::RegisterComponentModification(Entity entity,
											SerializedComponent componentBeforeModification,
											SerializedComponent componentAfterModification)
	{
		auto operation = std::make_unique<ComponentModificationOperation>();
		operation->pWorld = &entity.GetWorld();
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->SerializedComponentBeforeModification = componentBeforeModification;
		operation->SerializedComponentAfterModification = componentAfterModification;

		auto componentTypeName = operation->SerializedComponentBeforeModification.TryGetTypeName().value();

		sprintf(&operation->Description[0], "Modify %s of Entity '%s' (GUID: %s)",
				componentTypeName.c_str(),
				entity.TryGetName().value().c_str(),
				operation->EntityGUID.ToString().c_str());

		auto tryGetComponentTypeDescriptor = TypeRegistry::TryGetComponentTypeDescriptor(componentTypeName);
		DYE_ASSERT_LOG_WARN(tryGetComponentTypeDescriptor.has_value(),
							"Try to create a component modification operation but doesn't recognize the component type '%s'.", componentTypeName.c_str());

		operation->TypeDescriptor = tryGetComponentTypeDescriptor.value();

		pushNewOperation(std::move(operation));
	}

	void Undo::AddComponent(Entity entity, std::string const &componentTypeName, ComponentTypeDescriptor typeDescriptor)
	{
		auto operation = std::make_unique<ComponentAdditionOperation>();
		operation->pWorld = &entity.GetWorld();
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->TypeDescriptor = typeDescriptor;

		sprintf(&operation->Description[0], "Add %s to Entity '%s' (GUID: %s)",
				componentTypeName.c_str(),
				entity.TryGetName().value().c_str(),
				operation->EntityGUID.ToString().c_str());

		operation->TypeDescriptor.Add(entity);

		pushNewOperation(std::move(operation));
	}

	void Undo::RemoveComponent(Entity entity, const std::string &componentTypeName, ComponentTypeDescriptor typeDescriptor)
	{
		auto operation = std::make_unique<ComponentRemovalOperation>();
		operation->pWorld = &entity.GetWorld();
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->TypeDescriptor = typeDescriptor;
		operation->SerializedComponentBeforeRemoval = SerializedObjectFactory::CreateSerializedComponentOfType(entity, componentTypeName, typeDescriptor);

		sprintf(&operation->Description[0], "Remove %s from Entity '%s' (GUID: %s)",
				componentTypeName.c_str(),
				entity.TryGetName().value().c_str(),
				operation->EntityGUID.ToString().c_str());

		operation->TypeDescriptor.Remove(entity);

		pushNewOperation(std::move(operation));
	}

	void Undo::AddSystem(Scene &scene, SystemDescriptor systemDescriptor, int orderInList)
	{
		auto operation = std::make_unique<SystemAdditionOperation>();
		operation->Descriptor = systemDescriptor;
		operation->pScene = &scene;
		operation->OrderInSystemList = orderInList;

		if (systemDescriptor.Instance == nullptr)
		{
			// The added system is an unrecognized system.
			scene.UnrecognizedSystems.insert(scene.UnrecognizedSystems.begin() + orderInList, systemDescriptor);

			sprintf(operation->Description, "Add Unrecognized System '%s'", systemDescriptor.Name.c_str());
		}
		else
		{
			operation->ExecutionPhase = systemDescriptor.Instance->GetPhase();
			auto &systemDescriptors = scene.GetSystemDescriptorsOfPhase(operation->ExecutionPhase);
			systemDescriptors.insert(systemDescriptors.begin() + orderInList, systemDescriptor);

			sprintf(operation->Description, "Add System '%s'", systemDescriptor.Name.c_str());
		}

		pushNewOperation(std::move(operation));
	}

	void Undo::RemoveSystem(Scene &scene, SystemDescriptor systemDescriptor, int orderInList)
	{
		auto operation = std::make_unique<SystemRemovalOperation>();
		operation->Descriptor = systemDescriptor;
		operation->pScene = &scene;
		operation->OrderInSystemList = orderInList;

		if (systemDescriptor.Instance == nullptr)
		{
			sprintf(operation->Description, "Remove Unrecognized System '%s'", systemDescriptor.Name.c_str());

			// The removed system is an unrecognized system.
			scene.UnrecognizedSystems.erase(scene.UnrecognizedSystems.begin() + orderInList);
		}
		else
		{
			sprintf(operation->Description, "Remove System '%s'", systemDescriptor.Name.c_str());

			operation->ExecutionPhase = systemDescriptor.Instance->GetPhase();
			auto &systemDescriptors = scene.GetSystemDescriptorsOfPhase(operation->ExecutionPhase);
			systemDescriptors.erase(systemDescriptors.begin() + orderInList);
		}

		pushNewOperation(std::move(operation));
	}

	void Undo::SetSystemIsEnabled(Scene &scene, SystemDescriptor &systemDescriptor, int orderInList, bool value)
	{
		auto operation = std::make_unique<SetSystemIsEnabledOperation>();
		operation->pScene = &scene;
		operation->ExecutionPhase = systemDescriptor.Instance->GetPhase();
		operation->OrderInList = orderInList;
		operation->IsEnabledValue = value;

		systemDescriptor.IsEnabled = value;

		sprintf(operation->Description, "%s System '%s'", value? "Enable" : "Disable", systemDescriptor.Name.c_str());

		pushNewOperation(std::move(operation));
	}

	void Undo::ReorderSystem(Scene &scene, SystemDescriptor systemDescriptor, int oldOrderInList, int newOrderInList)
	{
		auto operation = std::make_unique<SystemReorderOperation>();
		operation->pScene = &scene;
		operation->pSystemBase = systemDescriptor.Instance;
		operation->OrderBeforeModification = oldOrderInList;
		operation->OrderAfterModification = newOrderInList;

		auto &systemList = scene.GetSystemDescriptorsOfPhase(operation->pSystemBase->GetPhase());
		auto const otherDescriptor = systemList[newOrderInList];
		systemList[newOrderInList] = systemList[oldOrderInList];
		systemList[oldOrderInList] = otherDescriptor;

		sprintf(operation->Description, "Reorder System '%s' from %d to %d (Originally occupied by '%s')",
				systemDescriptor.Name.c_str(),
				oldOrderInList,
				newOrderInList,
				otherDescriptor.Name.c_str());

		pushNewOperation(std::move(operation));
	}

	void Undo::pushNewOperation(std::unique_ptr<UndoOperationBase> operation)
	{
		// Discard everything behind the current head.
		s_Data.Operations.erase(s_Data.Operations.begin() + (s_Data.LatestOperationIndex + 1), s_Data.Operations.end());

		s_Data.Operations.emplace_back(std::move(operation));
		s_Data.LatestOperationIndex = s_Data.Operations.size() - 1;
	}

	void Undo::DrawUndoHistoryWindow(bool *pIsOpen)
	{
		// Set a default size for the window in case it has never been opened before.
		const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
								ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("Undo History", pIsOpen))
		{
			ImGui::End();
			return;
		}

		ImGui::Text("%d operations recorded.", s_Data.Operations.size());

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
		ImGui::BeginChild("Undo Operations Child Window", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Undo", nullptr, false, HasOperationToUndo()))
			{
				PerformUndo();
			}
			if (ImGui::MenuItem("Redo", nullptr, false, HasOperationToRedo()))
			{
				PerformRedo();
			}
			if (ImGui::MenuItem("Clear"))
			{
				ClearAll();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::BeginTable("Undo Operations Table", 1, ImGuiTableFlags_RowBg))
		{
			for (int i = s_Data.Operations.size() - 1; i >= 0; i--)
			{
				UndoOperationBase &operation = *s_Data.Operations[i];

				float const minRowHeight = 20;
				ImGui::TableNextRow(ImGuiTableRowFlags_None, minRowHeight);
				ImGui::TableNextColumn();
				bool const isLatestOperation = i == s_Data.LatestOperationIndex;
				if (isLatestOperation)
				{
					ImGui::Separator();
				}

				bool const isUndone = i > s_Data.LatestOperationIndex;
				if (isUndone) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

				ImGui::PushID(i);
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0, 0.5f));
				ImGuiSelectableFlags const flags = ImGuiSelectableFlags_SpanAllColumns;
				if (ImGui::Selectable(operation.GetDescription(), false, flags, ImVec2(0, minRowHeight)) && !isLatestOperation)
				{
					// If operation selectable clicked && it's not the latest operation,
					// We want to do a sequence of redo or undo.
					if (i > s_Data.LatestOperationIndex)
					{
						// Redo until latest operation index equals to i.
						for (int opIndex = s_Data.LatestOperationIndex + 1; opIndex <= i; opIndex++)
						{
							s_Data.Operations[opIndex]->Redo();
						}
						s_Data.LatestOperationIndex = i;
					}
					else if (i < s_Data.LatestOperationIndex)
					{
						// Undo until latest operation index equals to i + 1.
						for (int opIndex = s_Data.LatestOperationIndex; opIndex >= i + 1; opIndex--)
						{
							s_Data.Operations[opIndex]->Undo();
						}
						s_Data.LatestOperationIndex = i;
					}
				}
				if (ImGui::IsItemHovered() && operation.HasTooltip())
				{
					ImGui::BeginTooltip();
					operation.DrawTooltip();
					ImGui::EndTooltip();
				}

				ImGui::PopStyleVar();
				ImGui::PopID();

				if (isUndone) ImGui::PopStyleColor();
			}
			ImGui::EndTable();

			if (s_Data.LatestOperationIndex == -1)
			{
				ImGui::Separator();
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::End();
	}
}
