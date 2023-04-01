#include "ImGui/EditorImGuiUtil.h"

#include "ImGui/ImGuiUtil.h"
#include "Core/EditorSystem.h"
#include "FileSystem/FileSystem.h"

#include <imgui.h>
#include <filesystem>
#include <string>

namespace DYE::ImGuiUtil
{
	bool DrawExecutionPhaseControl(const std::string &label, DYE::DYEditor::ExecutionPhase &phase)
	{
		auto phaseValue = static_cast<int32_t>(phase);
		return DrawDropdown
			(
				label,
				phaseValue,
				// TODO: The list is pending to change, see ExecutionPhase for further information.
				{"Initialize", "Update", "FixedUpdate", "Render", "ImGui"}
			);
	}

	constexpr char const* AssetDirectory = "assets";
	constexpr char const* FilePathPopupId = "Select a file##FilePathPopup";
	std::filesystem::path FilePathPopupRootDirectory;
	std::filesystem::path FilePathPopupSelectedPath;
	std::filesystem::path FilePathPopupCurrentDirectory;

	void OpenFilePathPopup(const std::filesystem::path &rootDirectory, std::filesystem::path initiallySelectedFilePath)
	{
		FilePathPopupRootDirectory = rootDirectory;
		FilePathPopupSelectedPath = initiallySelectedFilePath;

		if (DYEditor::FileSystem::FileExists(initiallySelectedFilePath))
		{
			FilePathPopupCurrentDirectory = initiallySelectedFilePath.parent_path();
		}
		else
		{
			FilePathPopupCurrentDirectory = rootDirectory;
		}

		ImGui::OpenPopup(FilePathPopupId);
	}

	FilePathPopupResult DrawFilePathPopup(std::filesystem::path &outputPath, FilePathPopupParameters params)
	{
		FilePathPopupResult result = FilePathPopupResult::StillOpen;

		ImVec2 const defaultPopupWindowSize(360, 320);
		float const buttonHeight = ImGui::GetFrameHeightWithSpacing();
		ImGui::SetNextWindowSize(defaultPopupWindowSize, ImGuiCond_Appearing);
		ImGuiWindowFlags const popupFlags = ImGuiWindowFlags_None;
		if (ImGui::BeginPopupModal(FilePathPopupId, nullptr, popupFlags))
		{
			// Draw current directory as a sequence of folder selectables.
			// TODO: draw a folder icon here
			std::filesystem::path currentPathSequence = "";
			for (const auto &pathComponentItr: FilePathPopupCurrentDirectory)
			{
				currentPathSequence /= pathComponentItr;

				ImGui::SameLine();
				char const *pathComponent = pathComponentItr.string().c_str();
				ImVec2 const textSize = ImGui::CalcTextSize(pathComponent);
				ImGuiSelectableFlags const flags =
					ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowItemOverlap;
				if (ImGui::Selectable(pathComponentItr.string().c_str(), false, flags, textSize))
				{
					FilePathPopupCurrentDirectory = currentPathSequence;
					break;
				}
				ImGui::SameLine();
				ImGui::TextUnformatted(">");
			}

			ImGuiWindowFlags const childFlags = ImGuiWindowFlags_None;
			if (ImGui::BeginChild("File Browser", ImVec2(0, -buttonHeight), true, childFlags))
			{
				if (FilePathPopupCurrentDirectory != FilePathPopupRootDirectory)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
					if (ImGui::Selectable("...", false, ImGuiSelectableFlags_DontClosePopups))
					{
						FilePathPopupCurrentDirectory = FilePathPopupCurrentDirectory.parent_path();
					}
					ImGui::PopStyleColor();
				}

				for (auto &directoryEntry: std::filesystem::directory_iterator(FilePathPopupCurrentDirectory))
				{
					std::string const pathString = directoryEntry.path().string();
					std::string const fileNameString = directoryEntry.path().filename().string();
					if (directoryEntry.is_directory())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
						if (ImGui::Selectable(fileNameString.c_str(), false, ImGuiSelectableFlags_DontClosePopups))
						{
							FilePathPopupCurrentDirectory /= directoryEntry.path().filename();
						}
						ImGui::PopStyleColor();
					}
					else
					{
						bool const isFileSelected = FilePathPopupSelectedPath == directoryEntry.path();
						if (ImGui::Selectable(fileNameString.c_str(), isFileSelected, ImGuiSelectableFlags_DontClosePopups))
						{
							if (params.SaveOnFileClicked)
							{
								outputPath = directoryEntry.path();
								ImGui::CloseCurrentPopup();
								result = FilePathPopupResult::Save;
							}
							else
							{
								FilePathPopupSelectedPath = directoryEntry.path();
							}
						}
					}
				}
			}
			ImGui::EndChild();

			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
				result = FilePathPopupResult::Close;
			}

			if (!params.SaveOnFileClicked)
			{
				ImGui::SameLine();
				if (ImGui::Button("Save"))
				{
					outputPath = FilePathPopupSelectedPath;
					ImGui::CloseCurrentPopup();
					result = FilePathPopupResult::Save;
				}
			}

			ImGui::SameLine();
			ImGui::Text(FilePathPopupSelectedPath.string().c_str());

			ImGui::EndPopup();
		}

		return result;
	}
}
