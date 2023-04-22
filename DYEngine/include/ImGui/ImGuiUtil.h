#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>

namespace DYE::Math
{
	struct Rect;
	struct AABB;
}

namespace DYE
{
	class CameraProperties;
	class Material;
	class Texture2D;
	struct GUID;
}

namespace DYE::ImGuiUtil
{
	namespace Settings
	{
		constexpr float DefaultControlLabelWidth = 160.0f;
		extern float ControlLabelWidth;

		constexpr const char DefaultFloatFormat[] = "%.2f";
		extern std::string FloatFormat;

		void ResetParametersToDefaultValues();
	}

	bool DrawVector2Control(const std::string& label, glm::vec2& value, float resetValue = 0.0f);
	bool DrawVector3Control(const std::string& label, glm::vec3& value, float resetValue = 0.0f);
	bool DrawVector4Control(const std::string& label, glm::vec4& value, float resetValue = 0.0f);
	bool DrawBoolControl(const std::string& label, bool& value);
	bool DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f);
	bool DrawIntControl(const std::string& label, int32_t & value, int32_t resultValue = 0);
	bool DrawIntSliderControl(const std::string& label, int32_t & value, int32_t minValue, int32_t maxValue);
	bool DrawColor4Control(const std::string& label, glm::vec4& value);
	bool DrawTextControl(std::string const& label, std::string & text);
	bool DrawCharControl(std::string const& label, char& character);
	bool DrawAssetPathStringControl(std::string const &label, std::filesystem::path &path, std::vector<std::filesystem::path> const& extensions = {});
	bool DrawGUIDControl(std::string const &label, DYE::GUID &guid);

	void DrawReadOnlyTextWithLabel(std::string const& label, std::string const& text);
	bool DrawToolbar(const std::string& label, int32_t& value, std::vector<std::string> const& texts);
	//bool DrawDropdown(const std::string& label, int32_t& value, std::vector<char const*> const& texts);
	bool DrawDropdown(const std::string& label, int32_t& value, std::vector<std::string> const& texts);
	void DrawTexture2DPreviewWithLabel(std::string const& label, std::shared_ptr<Texture2D> const& texture);

	void DrawHelpMarker(const char* description);

	bool DrawRectControl(const std::string& label, Math::Rect& value, Math::Rect const& resetValue);
	bool DrawAABBControl(const std::string& label, Math::AABB& aabb);
	bool DrawCameraPropertiesControl(const std::string& label, CameraProperties& cameraProperties);
	bool DrawMaterialControl(const std::string& label, Material& material);

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
