#pragma once

#include <glm/glm.hpp>
#include <string>

namespace DYE
{
	class CameraProperties;
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
	bool DrawColor4Control(const std::string& label, glm::vec4& value);

	bool DrawCameraPropertiesControl(const std::string& label, CameraProperties& cameraProperties);
}
