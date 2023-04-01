#pragma once

#include <string>
#include <filesystem>

namespace DYE::DYEditor
{
	enum class ExecutionPhase;
}

namespace DYE::ImGuiUtil
{
	bool DrawExecutionPhaseControl(std::string const& label, DYE::DYEditor::ExecutionPhase& phase);

	void OpenFilePathPopup(char const* popupId,
						   std::filesystem::path const& rootDirectory,
						   std::filesystem::path const& initiallySelectedFilePath,
						   std::vector<std::filesystem::path> const& extensions = {});

	enum class FilePathPopupResult
	{
		StillOpen,
		Close,
		Save
	};

	struct FilePathPopupParameters
	{
		/// If set to true, the file path is saved directly when clicked.\n
		/// If set to false, the user has to press 'Save' button after selecting a file to actually save the file path.
		bool SaveOnFileClicked = true;

		/// If set to true, files that are filtered will still be shown in the browser but disabled.\n
		/// If set to false, filtered files won't be shown in the browser.
		bool ShowFilteredFilesAsDisabled = true;
	};

	FilePathPopupResult DrawFilePathPopup(char const* popupId, std::filesystem::path& outputPath, FilePathPopupParameters params = {});
}