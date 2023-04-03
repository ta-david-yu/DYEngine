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
		Cancel,
		Confirm
	};

	struct FilePathPopupParameters
	{
		bool IsSaveFilePanel = false;

		/// If set to true, files that are filtered will still be shown in the browser but disabled.\n
		/// If set to false, filtered files won't be shown in the browser.
		bool ShowFilteredFilesAsDisabled = true;

		/// This parameter is only valid when IsSaveFilePanel is true.
		/// The file extension will be appended to the user's filename input automatically on confirm.
		const char* SaveFileExtension = nullptr;

		/// This parameter is only valid when IsSaveFilePanel is true.
		/// Show a popup which warns the user that a file with the given filename has already existed.
		bool ShowOverwritePopupOnConfirmSave = true;
	};

	FilePathPopupResult DrawFilePathPopup(char const* popupId, std::filesystem::path& outputPath, FilePathPopupParameters params = {});
}