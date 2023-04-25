#include "Undo/Undo.h"

#include "Undo/UndoOperationBase.h"
#include "Undo/Operations/EntityOperations.h"
#include "Undo/Operations/SystemOperations.h"

#include "Serialization/SerializedObjectFactory.h"
#include "ImGui/ImGuiUtil.h"

#include <memory>
#include <vector>

namespace DYE::DYEditor
{
	struct UndoData
	{
		std::vector<std::unique_ptr<UndoOperationBase>> Operations;
		int LatestOperationIndex = -1;
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

	void Undo::RegisterEntityCreation(World &world, Entity &entity)
	{
		auto tryGetIndexResult = world.TryGetEntityIndex(entity);
		RegisterEntityCreation(world, entity, tryGetIndexResult.has_value()? tryGetIndexResult.value() : 0);
	}

	void Undo::RegisterEntityCreation(World &world, Entity &entity, std::size_t indexInWorldHandleArray)
	{
		auto operation = std::make_unique<EntityCreationOperation>();
		operation->pWorld = &world;
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->CreatedSerializedEntity = SerializedObjectFactory::CreateSerializedEntity(entity);
		operation->IndexInWorldEntityArray = indexInWorldHandleArray;

		sprintf(operation->Description, "Create New Entity (GUID: %s)", operation->EntityGUID.ToString().c_str());

		pushNewOperation(std::move(operation));
	}

	void Undo::DeleteEntity(World &world, Entity &entity)
	{
		auto tryGetIndexResult = world.TryGetEntityIndex(entity);
		DeleteEntity(world, entity, tryGetIndexResult.has_value()? tryGetIndexResult.value() : 0);
	}

	void Undo::DeleteEntity(World &world, Entity &entity, std::size_t indexInWorldHandleArray)
	{
		auto operation = std::make_unique<EntityDeletionOperation>();
		operation->pWorld = &world;
		operation->EntityGUID = entity.GetComponent<IDComponent>().ID;
		operation->DeletedSerializedEntity = SerializedObjectFactory::CreateSerializedEntity(entity);
		operation->IndexInWorldEntityArray = indexInWorldHandleArray;

		sprintf(operation->Description, "Delete Entity '%s' (GUID: %s)", entity.TryGetName().value().c_str(), operation->EntityGUID.ToString().c_str());

		operation->pWorld->DestroyEntityWithGUID(operation->EntityGUID);

		pushNewOperation(std::move(operation));
	}

	void Undo::RegisterComponentModification(Entity &entity,
											SerializedComponent componentBeforeModification,
											SerializedComponent componentAfterModification)
	{
		auto operation = std::make_unique<ComponentModificationOperation>(entity, std::move(componentBeforeModification), std::move(componentAfterModification));
		pushNewOperation(std::move(operation));
	}

	void Undo::AddComponent(Entity &entity, std::string const &componentTypeName, ComponentTypeDescriptor typeDescriptor)
	{
		auto operation = std::make_unique<ComponentAdditionOperation>(entity, componentTypeName, typeDescriptor);
		pushNewOperation(std::move(operation));
	}

	void Undo::RemoveComponent(Entity &entity, const std::string &componentTypeName, ComponentTypeDescriptor typeDescriptor)
	{
		auto operation = std::make_unique<ComponentRemovalOperation>(entity, componentTypeName, typeDescriptor);
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
		s_Data.Operations.erase(s_Data.Operations.begin() + s_Data.LatestOperationIndex + 1, s_Data.Operations.end());

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
