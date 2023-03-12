#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace DYE::Math
{
	struct Rect;
	struct AABB;
}

namespace DYE
{
	class CameraProperties;
	class Material;
}

namespace DYE::ImGuiUtil
{
	namespace Parameters
	{
		constexpr float DefaultControlLabelWidth = 200.0f;
		extern float ControlLabelWidth;

		constexpr const char DefaultFloatFormat[] = "%.2f";
		extern std::string FloatFormat;

		void ResetParametersToDefaultValues();
	}

	bool DrawVector2Control(const std::string& label, glm::vec2& value, float resetValue = 0.0f);
	bool DrawVector3Control(const std::string& label, glm::vec3& value, float resetValue = 0.0f);
	bool DrawBoolControl(const std::string& label, bool& value);
	bool DrawFloatControl(const std::string& label, float& value, float resetValue = 0.0f);
	bool DrawIntControl(const std::string& label, int32_t & value, int32_t resultValue = 0);
	bool DrawColor4Control(const std::string& label, glm::vec4& value);
	bool DrawRectControl(const std::string& label, Math::Rect& value, Math::Rect const& resetValue);
	bool DrawToolbar(const std::string& label, int32_t& value, std::vector<std::string> const& texts);
	//bool DrawDropdown(const std::string& label, int32_t& value, std::vector<char const*> const& texts);
	bool DrawDropdown(const std::string& label, int32_t& value, std::vector<std::string> const& texts);

	bool DrawTextControl(std::string const& label, std::string & text);

	void DrawReadOnlyTextWithLabel(std::string const& label, std::string const& text);

	bool DrawAABBControl(const std::string& label, Math::AABB& aabb);
	bool DrawCameraPropertiesControl(const std::string& label, CameraProperties& cameraProperties);
	bool DrawMaterialControl(const std::string& label, Material& material);
}
