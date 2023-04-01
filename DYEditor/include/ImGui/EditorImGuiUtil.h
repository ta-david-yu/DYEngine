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

	void OpenFilePathPopup(const std::filesystem::path &rootDirectory, std::filesystem::path initiallySelectedFilePath);

	enum class FilePathPopupResult
	{
		StillOpen,
		Close,
		Save
	};

	struct FilePathPopupParameters
	{
		/// Save the selected file path when clicking on the file directly. If it's set to false, the user has to press 'Save' button after selecting a file
		/// to actually save the file path.
		bool SaveOnFileClicked = true;
	};

	FilePathPopupResult DrawFilePathPopup(std::filesystem::path& outputPath, FilePathPopupParameters params = {});
}