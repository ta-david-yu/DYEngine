#pragma once

#include "glm/glm.hpp"
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

		void ResetParametersToDefaultValues();
	}

	/// Draw a glm::vec3 variable imgui handle.
	/// \param label
	/// \param value
	/// \param resetValue
	/// \param columnWidth
	/// \return true if the value is changed.
	bool DrawVec3Control(const std::string& label, glm::vec3& value, float resetValue = 0.0f);
	bool DrawBooleanControl(const std::string& label, bool& value);
	bool DrawFloatControl(const std::string& label, float& value, float resetValue);
	bool DrawUnsignedIntControl(const std::string& label, int32_t & value, int32_t resultValue);
	bool DrawColor4Control(const std::string& label, glm::vec4& value);
	bool DrawRectControl(const std::string& label, Math::Rect& value, Math::Rect const& resetValue);
	bool DrawToolbar(const std::string& label, int32_t& value, std::vector<std::string> const& texts);
	bool DrawDropdown(const std::string& label, int32_t& value, std::vector<char const*> const& texts);

	void DrawReadOnlyTextWithLabel(std::string const& label, std::string const& text);

	bool DrawAABBControl(const std::string& label, Math::AABB& aabb);
	bool DrawCameraPropertiesControl(const std::string& label, CameraProperties& cameraProperties);
	bool DrawMaterialControl(const std::string& label, Material& material);
}
