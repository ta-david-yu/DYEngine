#include "ImGui/EditorImGuiUtil.h"

#include "ImGui/ImGuiUtil.h"
#include "Core/EditorSystem.h"
#include "FileSystem/FileSystem.h"

#include <imgui.h>
#include <imgui_stdlib.h>
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

		ImVec2 const defaultPopupWindowSize(440, 320);
		float const selectedFilepathTextHeight = ImGui::GetFrameHeightWithSpacing();
		float const buttonHeight = ImGui::GetFrameHeightWithSpacing();
		ImGui::SetNextWindowSize(defaultPopupWindowSize, ImGuiCond_Appearing);
		ImGuiWindowFlags const popupFlags = ImGuiWindowFlags_None;

		if (!ImGui::BeginPopupModal(popupId, nullptr, popupFlags))
		{
			return result;
		}

		char const* confirmSaveAsPopupId = "Confirm Save As";
		bool openConfirmSaveAsPopup = false;

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
		if (ImGui::BeginChild("File Browser", ImVec2(0, -buttonHeight - selectedFilepathTextHeight), true, childFlags))
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
					ImGuiSelectableFlags const flags = ImGuiSelectableFlags_DontClosePopups;
					if (ImGui::Selectable(fileNameString.c_str(), false, flags))
					{
						FilePathPopup_CurrentDirectory /= directoryEntry.path().filename();
					}
					ImGui::PopStyleColor();
				}
				else
				{
					// Draw file.
					bool isFileExcludedByExtension = false;
					if (FilePathPopup_HasExtensionFilter)
					{
						auto const &fileExtension = directoryEntry.path().extension();
						isFileExcludedByExtension = !FilePathPopup_Extensions.contains(fileExtension);
					}

					ImGuiSelectableFlags flags =
						ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick;
					bool drawFileSelectable = true;
					if (isFileExcludedByExtension)
					{
						if (params.ShowFilteredFilesAsDisabled)
						{
							flags |= ImGuiSelectableFlags_Disabled;
						}
						else
						{
							drawFileSelectable = false;
						}
					}

					if (!drawFileSelectable)
					{
						continue;
					}

					bool const isFileSelected = FilePathPopup_SelectedFilePath == directoryEntry.path();
					if (ImGui::Selectable(fileNameString.c_str(), isFileSelected, flags))
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							outputPath = directoryEntry.path();
							if (params.IsSaveFilePanel && DYEditor::FileSystem::FileExists(outputPath))
							{
								// We store a flag here and delay opening the popup
								// because MenuItem is Selectable and Selectable by default calls CloseCurrentPopup().
								openConfirmSaveAsPopup = true;
							}
							else
							{
								ImGui::CloseCurrentPopup();
								result = FilePathPopupResult::Confirm;
							}
						}
						else
						{
							FilePathPopup_SelectedFilePath = directoryEntry.path();

							bool const hasAutoFileExtension = params.SaveFileExtension != nullptr;
							if (params.IsSaveFilePanel && hasAutoFileExtension)
							{
								// Remove the extension if auto file extension is provided.
								FilePathPopup_SelectedFilePath.replace_extension("");
							}
						}
					}
				}
			}
		}
		ImGui::EndChild();

		if (params.IsSaveFilePanel)
		{
			std::string filenameAsString = FilePathPopup_SelectedFilePath.filename().string();
			bool const changed = ImGui::InputText("##SaveFilename", &filenameAsString);
			if (changed)
			{
				FilePathPopup_SelectedFilePath.replace_filename(filenameAsString);
			}

			bool const hasAutoFileExtension = params.SaveFileExtension != nullptr;
			if (hasAutoFileExtension)
			{
				ImGui::SameLine();
				ImGui::TextUnformatted(params.SaveFileExtension);
			}
		}
		else
		{
			ImGui::Text(FilePathPopup_SelectedFilePath.string().c_str());
		}

		float const buttonPadding = 10;
		ImVec2 const buttonSize = ImVec2 {75, buttonHeight};
		float const scrollBarWidth = ImGui::GetCurrentWindow()->ScrollbarY ? ImGui::GetWindowScrollbarRect(
			ImGui::GetCurrentWindow(), ImGuiAxis_Y).GetWidth() : 0;
		float const availableWidthForAddButton = ImGui::GetWindowWidth() - scrollBarWidth;
		ImGui::SetCursorPosX(availableWidthForAddButton - buttonSize.x - buttonPadding);
		if (ImGui::Button("Cancel", buttonSize))
		{
			ImGui::CloseCurrentPopup();
			result = FilePathPopupResult::Cancel;
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(availableWidthForAddButton - buttonSize.x * 2 - buttonPadding * 2);

		bool closeMainModalPopupAfterConfirmSaveAsPopup = false;
		if (params.IsSaveFilePanel)
		{
			// For a save panel, we need to do some filename extension checking
			if (ImGui::Button("Confirm", buttonSize))
			{
				bool const hasAutoFileExtension = params.SaveFileExtension != nullptr;
				auto selectedFileExtension = FilePathPopup_SelectedFilePath.extension();
				if (hasAutoFileExtension && selectedFileExtension != params.SaveFileExtension)
				{
					// Append auto extension file if the filepath doesn't have a matching one.
					FilePathPopup_SelectedFilePath += params.SaveFileExtension;
				}
				// The final save directory depends on the current directory.
				outputPath = FilePathPopup_CurrentDirectory / FilePathPopup_SelectedFilePath.filename();

				if (DYEditor::FileSystem::FileExists(outputPath))
				{
					// We store a flag here and delay opening the popup
					// because MenuItem is Selectable and Selectable by default calls CloseCurrentPopup().
					openConfirmSaveAsPopup = true;
				}
				else
				{
					ImGui::CloseCurrentPopup();
					result = FilePathPopupResult::Confirm;
				}
			}

			if (openConfirmSaveAsPopup)
			{
				// Open duplicate filename warning popup.
				ImGui::OpenPopup(confirmSaveAsPopupId);
			}

			if (ImGui::BeginPopupModal(confirmSaveAsPopupId))
			{
				ImGui::Text("'%s' already exists. Do you want to replace it?", outputPath.filename().string().c_str());
				if (ImGui::Button("Yes"))
				{
					ImGui::CloseCurrentPopup();
					closeMainModalPopupAfterConfirmSaveAsPopup = true;
					result = FilePathPopupResult::Confirm;
				}
				ImGui::SameLine();
				if (ImGui::Button("No"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		else
		{
			bool const isSelectedFilePathValid = DYEditor::FileSystem::FileExists(FilePathPopup_SelectedFilePath);

			ImGui::BeginDisabled(!isSelectedFilePathValid);
			if (ImGui::Button("Confirm", buttonSize))
			{
				outputPath = FilePathPopup_SelectedFilePath;
				ImGui::CloseCurrentPopup();
				result = FilePathPopupResult::Confirm;
			}
			ImGui::EndDisabled();
		}

		if (closeMainModalPopupAfterConfirmSaveAsPopup)
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();

		return result;
	}
}
