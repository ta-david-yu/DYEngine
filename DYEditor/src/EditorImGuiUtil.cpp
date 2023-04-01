#include "ImGui/EditorImGuiUtil.h"

#include "ImGui/ImGuiUtil.h"
#include "Core/EditorSystem.h"
#include "FileSystem/FileSystem.h"

#include <imgui.h>
#include <filesystem>
#include <string>
#include <set>

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

	std::filesystem::path FilePathPopup_RootDirectory;
	std::filesystem::path FilePathPopup_SelectedFilePath;
	std::filesystem::path FilePathPopup_CurrentDirectory;
	bool FilePathPopup_HasExtensionFilter = false;
	std::set<std::filesystem::path> FilePathPopup_Extensions;

	void OpenFilePathPopup(char const* popupId,
						   const std::filesystem::path &rootDirectory,
						   const std::filesystem::path& initiallySelectedFilePath,
						   std::vector<std::filesystem::path> const& extensions)
	{
		FilePathPopup_RootDirectory = rootDirectory;
		FilePathPopup_SelectedFilePath = initiallySelectedFilePath;

		if (DYEditor::FileSystem::FileExists(initiallySelectedFilePath))
		{
			FilePathPopup_CurrentDirectory = initiallySelectedFilePath.parent_path();
		}
		else
		{
			FilePathPopup_CurrentDirectory = rootDirectory;
		}

		FilePathPopup_HasExtensionFilter = !extensions.empty();
		if (FilePathPopup_HasExtensionFilter)
		{
			FilePathPopup_Extensions = std::set<std::filesystem::path>(extensions.begin(), extensions.end());
		}

		ImGui::OpenPopup(popupId);
	}

	FilePathPopupResult DrawFilePathPopup(char const* popupId, std::filesystem::path &outputPath, FilePathPopupParameters params)
	{
		FilePathPopupResult result = FilePathPopupResult::StillOpen;

		ImVec2 const defaultPopupWindowSize(360, 320);
		float const buttonHeight = ImGui::GetFrameHeightWithSpacing();
		ImGui::SetNextWindowSize(defaultPopupWindowSize, ImGuiCond_Appearing);
		ImGuiWindowFlags const popupFlags = ImGuiWindowFlags_None;
		if (ImGui::BeginPopupModal(popupId, nullptr, popupFlags))
		{
			// TODO: draw a folder icon here
			// Draw current directory as a sequence of folder selectables.
			std::filesystem::path currentPathSequence = "";
			for (const auto &pathComponentItr: FilePathPopup_CurrentDirectory)
			{
				currentPathSequence /= pathComponentItr;

				ImGui::SameLine();
				char const *pathComponent = pathComponentItr.string().c_str();
				ImVec2 const textSize = ImGui::CalcTextSize(pathComponent);
				ImGuiSelectableFlags const flags =
					ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowItemOverlap;
				if (ImGui::Selectable(pathComponentItr.string().c_str(), false, flags, textSize))
				{
					FilePathPopup_CurrentDirectory = currentPathSequence;
					break;
				}
				ImGui::SameLine();
				ImGui::TextUnformatted(">");
			}

			ImGuiWindowFlags const childFlags = ImGuiWindowFlags_None;
			if (ImGui::BeginChild("File Browser", ImVec2(0, -buttonHeight), true, childFlags))
			{
				if (FilePathPopup_CurrentDirectory != FilePathPopup_RootDirectory)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
					if (ImGui::Selectable("...", false, ImGuiSelectableFlags_DontClosePopups))
					{
						FilePathPopup_CurrentDirectory = FilePathPopup_CurrentDirectory.parent_path();
					}
					ImGui::PopStyleColor();
				}

				for (auto &directoryEntry: std::filesystem::directory_iterator(FilePathPopup_CurrentDirectory))
				{
					std::string const pathString = directoryEntry.path().string();
					std::string const fileNameString = directoryEntry.path().filename().string();
					if (directoryEntry.is_directory())
					{
						// Draw directory.
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
						if (ImGui::Selectable(fileNameString.c_str(), false, ImGuiSelectableFlags_DontClosePopups))
						{
							FilePathPopup_CurrentDirectory /= directoryEntry.path().filename();
						}
						ImGui::PopStyleColor();
					}
					else
					{
						// Draw file.
						bool isExtensionExcluded = false;
						if (FilePathPopup_HasExtensionFilter)
						{
							auto const& fileExtension = directoryEntry.path().extension();
							isExtensionExcluded = !FilePathPopup_Extensions.contains(fileExtension);
						}

						if (!isExtensionExcluded)
						{
							bool const isFileSelected = FilePathPopup_SelectedFilePath == directoryEntry.path();
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
									FilePathPopup_SelectedFilePath = directoryEntry.path();
								}
							}
						}
						else if (params.ShowFilteredFilesAsDisabled)
						{
							bool const isFileSelected = FilePathPopup_SelectedFilePath == directoryEntry.path();
							if (ImGui::Selectable(fileNameString.c_str(), isFileSelected, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_Disabled))
							{
								if (params.SaveOnFileClicked)
								{
									outputPath = directoryEntry.path();
									ImGui::CloseCurrentPopup();
									result = FilePathPopupResult::Save;
								}
								else
								{
									FilePathPopup_SelectedFilePath = directoryEntry.path();
								}
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
					outputPath = FilePathPopup_SelectedFilePath;
					ImGui::CloseCurrentPopup();
					result = FilePathPopupResult::Save;
				}
			}

			ImGui::SameLine();
			ImGui::Text(FilePathPopup_SelectedFilePath.string().c_str());

			ImGui::EndPopup();
		}

		return result;
	}
}
