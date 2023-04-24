#include "Undo/Undo.h"

#include "Undo/UndoOperationBase.h"
#include "Undo/Operations/EntityOperations.h"

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

	void
	Undo::RegisterComponentModification(World &world, Entity &entity, SerializedComponent componentBeforeModification,
										SerializedComponent componentAfterModification)
	{
		// TODO:
	}

	void Undo::RegisterEntityCreation(World &world, Entity &entity)
	{
		auto tryGetIndexResult = world.TryGetEntityIndex(entity);
		RegisterEntityCreation(world, entity, tryGetIndexResult.has_value()? tryGetIndexResult.value() : 0);
	}

	void Undo::RegisterEntityCreation(World &world, Entity &entity, std::size_t indexInWorldHandleArray)
	{
		// Discard everything behind the current head.
		s_Data.Operations.erase(s_Data.Operations.begin() + s_Data.LatestOperationIndex + 1, s_Data.Operations.end());

		auto operation = std::make_unique<EntityCreationOperation>(world, entity);
		operation->m_IndexInWorldEntityArray = indexInWorldHandleArray;

		s_Data.Operations.emplace_back(std::move(operation));
		s_Data.LatestOperationIndex = s_Data.Operations.size() - 1;
	}

	void Undo::DeleteEntity(World &world, Entity &entity)
	{
		auto tryGetIndexResult = world.TryGetEntityIndex(entity);
		DeleteEntity(world, entity, tryGetIndexResult.has_value()? tryGetIndexResult.value() : 0);
	}

	void Undo::DeleteEntity(World &world, Entity &entity, std::size_t indexInWorldHandleArray)
	{
		// Discard everything behind the current head.
		s_Data.Operations.erase(s_Data.Operations.begin() + s_Data.LatestOperationIndex + 1, s_Data.Operations.end());

		auto operation = std::make_unique<EntityDeletionOperation>(world, entity);
		operation->m_IndexInWorldEntityArray = indexInWorldHandleArray;

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

		if (!ImGui::Begin("Undo History", pIsOpen, ImGuiWindowFlags_MenuBar))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::MenuItem("Undo"))
			{
				PerformUndo();
			}
			if (ImGui::MenuItem("Redo"))
			{
				PerformRedo();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTable("Undo Operations Table", 1, ImGuiTableFlags_RowBg))
		{
			for (int i = s_Data.Operations.size() - 1; i >= 0; i--)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				UndoOperationBase &operation = *s_Data.Operations[i];
				bool const isLatestOperation = i == s_Data.LatestOperationIndex;
				if (isLatestOperation)
				{
					ImGui::Separator();
				}

				bool const isUndone = i > s_Data.LatestOperationIndex;
				if (isUndone) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

				if (ImGui::Selectable(operation.GetDescription()) && !isLatestOperation)
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

				if (isUndone) ImGui::PopStyleColor();
			}
			ImGui::EndTable();

			if (s_Data.LatestOperationIndex == -1)
			{
				ImGui::Separator();
			}
		}

		ImGui::End();
	}
}
