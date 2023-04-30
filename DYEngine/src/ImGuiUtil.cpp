#include "ImGui/ImGuiUtil.h"
#include "ImGui/ImGuiUtil_Internal.h"

#include "FileSystem/FileSystem.h"
#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Texture.h"
#include "Graphics/Shader.h"
#include "Math/AABB.h"
#include "Core/GUID.h"

#include <set>
#include <imgui.h>
#include <imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace DYE::ImGuiUtil
{
	namespace Settings
	{
		float ControlLabelWidth = DefaultControlLabelWidth;
		std::string FloatFormat = DefaultFloatFormat;

		void ResetParametersToDefaultValues()
		{
			ControlLabelWidth = DefaultControlLabelWidth;
			FloatFormat = DefaultFloatFormat;
		}
	}

	struct ControlContext
	{
		bool IsLastControlActivated = false;
		bool IsLastControlDeactivated = false;
		bool IsLastControlDeactivatedAfterEdit = false;

		void Reset()
		{
			IsLastControlActivated = false;
			IsLastControlDeactivated = false;
			IsLastControlDeactivatedAfterEdit = false;
		}

		void UpdateLastControlState()
		{
			IsLastControlActivated |= ImGui::IsItemActivated();
			IsLastControlDeactivated |= ImGui::IsItemDeactivated();
			IsLastControlDeactivatedAfterEdit |= ImGui::IsItemDeactivatedAfterEdit();
		}
	};

	static ControlContext s_Context;

	bool IsControlActivated()
	{
		return s_Context.IsLastControlActivated;
	}

	bool IsControlDeactivated()
	{
		return s_Context.IsLastControlDeactivated;
	}

	bool IsControlDeactivatedAfterEdit()
	{
		return s_Context.IsLastControlDeactivatedAfterEdit;
	}

	bool DrawVector2Control(const std::string &label, glm::vec2 &value, float resetValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				value.x = resetValue;
				isValueChanged |= true;
				s_Context.IsLastControlDeactivatedAfterEdit = true;
			}
			s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
			s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			isValueChanged |= ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
			s_Context.UpdateLastControlState();
			ImGui::PopItemWidth();
		}

		ImGui::SameLine();

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.8f, 0.3f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
			ImGui::PushFont(boldFont);
			if (ImGui::Button("Y", buttonSize))
			{
				value.y = resetValue;
				isValueChanged |= true;
				s_Context.IsLastControlDeactivatedAfterEdit = true;
			}
			s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
			s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			isValueChanged |= ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
			s_Context.UpdateLastControlState();
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawVector3Control(const std::string &label, glm::vec3 &value, float resetValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value.x = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value.y = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			value.z = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawVector4Control(const std::string &label, glm::vec4 &value, float resetValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value.x = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value.y = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			value.z = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();;
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize))
		{
			value.w = resetValue;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##W", &value.w, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawBoolControl(const std::string &label, bool &value)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		isValueChanged |= ImGui::Checkbox("##Boolean", &value);
		s_Context.UpdateLastControlState();

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawFloatControl(const std::string &label, float &value, float resetValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		// Drag Float
		{
			isValueChanged |= ImGui::DragFloat("##Float", &value, 0.1f, 0, 0, Settings::FloatFormat.c_str());
			s_Context.UpdateLastControlState();
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// Reset button
		{
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

			///ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			///ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
			///ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			ImGui::PushFont(boldFont);
			if (ImGui::Button("R", buttonSize))
			{
				value = resetValue;
				isValueChanged |= true;
				s_Context.IsLastControlDeactivatedAfterEdit = true;
			}
			s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
			s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
			ImGui::PopFont();
			///ImGui::PopStyleColor(3);
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawIntControl(const std::string &label, int32_t &value, int32_t resetValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		// Drag int
		{
			isValueChanged |= ImGui::DragInt("##SignedInt", &value, 1, 0, 0);
			s_Context.UpdateLastControlState();
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		// Reset button
		{
			float const lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImVec2 const buttonSize = {lineHeight + 3.0f, lineHeight};

			///ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			///ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
			///ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			ImGui::PushFont(boldFont);
			if (ImGui::Button("R", buttonSize))
			{
				value = resetValue;
				isValueChanged |= true;
				s_Context.IsLastControlDeactivatedAfterEdit = true;
			}
			s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
			s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
			ImGui::PopFont();
			///ImGui::PopStyleColor(3);
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}


	bool DrawIntSliderControl(const std::string& label, int32_t & value, int32_t minValue, int32_t maxValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		// Drag int
		{
			isValueChanged |= ImGui::SliderInt("##IntSlider", &value, minValue, maxValue);
			s_Context.UpdateLastControlState();
			ImGui::PopItemWidth();
			ImGui::SameLine();
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawColor4Control(const std::string &label, glm::vec4 &value)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		isValueChanged |= ImGui::ColorEdit4("##Color", glm::value_ptr(value));
		s_Context.UpdateLastControlState();

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawRectControl(const std::string &label, Math::Rect &value, Math::Rect const &resetValue)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 const buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value.X = resetValue.X;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##X", &value.X, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value.Y = resetValue.Y;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Y", &value.Y, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize))
		{
			value.Width = resetValue.Width;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##W", &value.Width, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("H", buttonSize))
		{
			value.Height = resetValue.Height;
			isValueChanged |= true;
			s_Context.IsLastControlDeactivatedAfterEdit = true;
		}
		s_Context.IsLastControlActivated |= ImGui::IsItemActivated();
		s_Context.IsLastControlDeactivated |= ImGui::IsItemDeactivated();
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##H", &value.Height, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
		s_Context.UpdateLastControlState();
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawTextControl(std::string const &label, std::string &text)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			isValueChanged |= ImGui::InputText("##text", &text);
			s_Context.UpdateLastControlState();
			ImGui::PopItemWidth();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawCharControl(const std::string &label, char &character)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			char buffer[2] = "";
			buffer[0] = character;
			isValueChanged |= ImGui::InputText("##character", buffer, 2);
			s_Context.UpdateLastControlState();
			if (isValueChanged)
			{
				character = buffer[0];
			}
			ImGui::PopItemWidth();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawAssetPathStringControl(std::string const &label, std::filesystem::path &path, std::vector<std::filesystem::path> const& extensions)
	{
		s_Context.Reset();

		bool changed = false;

		ImGui::PushID(label.c_str());

		std::filesystem::path const previousPath = path;
		auto pathAsString = path.string();
		bool isPathChanged = ImGuiUtil::DrawTextControl(label, pathAsString);
		s_Context.UpdateLastControlState();
		if (isPathChanged)
		{
			path = pathAsString;
		}
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 const buttonSize = {lineHeight + 3.0f, lineHeight};

		char const *popupId = "Select a file";
		ImGui::SameLine();
		if (ImGui::Button("S", buttonSize))
		{
			ImGuiUtil::OpenFilePathPopup(popupId, "assets", path, extensions);
			s_Context.IsLastControlActivated |= true;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted("Open a path selection window");
			ImGui::EndTooltip();
		}

		ImGuiUtil::FilePathPopupResult result = ImGuiUtil::DrawFilePathPopup(popupId, path);
		if (result == ImGuiUtil::FilePathPopupResult::Confirm)
		{
			s_Context.IsLastControlDeactivated = true;
			if (path != previousPath)
			{
				isPathChanged = true;
				s_Context.IsLastControlDeactivatedAfterEdit = true;
			}
		}

		changed |= isPathChanged;
		ImGui::PopID();

		return changed;
	}

	bool DrawGUIDControl(std::string const &label, DYE::GUID &guid)
	{
		s_Context.Reset();

		bool changed = false;

		auto guidString =  guid.ToString();
		changed |= ImGuiUtil::DrawTextControl(label, guidString);
		s_Context.UpdateLastControlState();
		if (changed)
		{
			try
			{
				guid = (GUID) std::stoull(guidString);
			}
			catch (std::exception const& e)
			{
				// If string failed to convert to guid, reset the changes.
				changed = false;
			}
		}

		return changed;
	}

	bool DrawToolbarControl(const std::string &label, int32_t &value, std::vector<std::string> const &texts)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(texts.size(), ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2 {0.5f, 0.5f});
		if (ImGui::BeginTable("Toolbar", texts.size(), ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableNextRow();
			for (int i = 0; i < texts.size(); ++i)
			{
				ImGui::TableSetColumnIndex(i);
				if (ImGui::Selectable(texts[i].c_str(), i == value, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowItemOverlap))
				{
					value = i;
					isValueChanged = true;
				}
				s_Context.UpdateLastControlState();
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleVar(2);
		ImGui::Columns(1);
		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawDropdownControl(const std::string &label, int32_t &value, std::vector<std::string> const &texts)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			if (ImGui::BeginCombo("##combo", texts[value].c_str()))
			{
				for (int i = 0; i < texts.size(); ++i)
				{
					bool const isSelected = value == i;
					if (ImGui::Selectable(texts[i].c_str(), isSelected))
					{
						if (i != value)
						{
							isValueChanged |= true;
						}
						value = i;
					}
					s_Context.UpdateLastControlState();

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			ImGui::PopItemWidth();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		return isValueChanged;
	}

	bool DrawAABBControl(const std::string& label, Math::AABB& aabb)
	{
		s_Context.Reset();

		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		{
			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
			{
				ImGui::LabelText("", "Min");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MinX", &aabb.Min.x, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
				s_Context.UpdateLastControlState();
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MinY", &aabb.Min.y, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
				s_Context.UpdateLastControlState();
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MinZ", &aabb.Min.z, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
				s_Context.UpdateLastControlState();
				ImGui::PopItemWidth();
			}
			ImGui::PopStyleVar();
		}

		{
			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
			{
				ImGui::LabelText("", "Max");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MaxX", &aabb.Max.x, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
				s_Context.UpdateLastControlState();
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MaxY", &aabb.Max.y, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
				s_Context.UpdateLastControlState();
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MaxZ", &aabb.Max.z, 0.1f, 0.0f, 0.0f, Settings::FloatFormat.c_str());
				s_Context.UpdateLastControlState();
				ImGui::PopItemWidth();
			}
			ImGui::PopStyleVar();
		}

		{
			ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
			{
				auto const center = aabb.Center();
				ImGui::LabelText("", "Center (%.2f, %.2f, %.2f)", center.x, center.y, center.z);
				ImGui::PopItemWidth();
			}
			ImGui::PopStyleVar();
		}

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	const static std::vector<std::string> s_WindowIndexDropDownOptions =
		{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
		 "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};

	bool DrawCameraPropertiesControl(const std::string& label, CameraProperties& cameraProperties)
	{
		s_Context.Reset();
		// We use a local variable to keep track of it because this Control is a compound control.
		// We will set the value to s_Context.LastControlDeactivatedAfterEdit at the end.
		bool lastControlActivated = false;
		bool lastControlDeactivated = false;
		bool lastControlDeactivatedAfterEdit = false;

		bool isValueChanged = false;

		ImGui::PushID(label.c_str());

		isValueChanged |= DrawColor4Control("Clear Color", cameraProperties.ClearColor);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

		isValueChanged |= DrawFloatControl("Depth ", cameraProperties.Depth);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

		ImGui::Separator();

		isValueChanged |= DrawBoolControl("Is Orthographic", cameraProperties.IsOrthographic);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

		if (cameraProperties.IsOrthographic)
		{
			isValueChanged |= DrawFloatControl("Orthographic Size", cameraProperties.OrthographicSize, 1);
			lastControlActivated |= ImGuiUtil::IsControlActivated();
			lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
			lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
		}
		else
		{
			isValueChanged |= DrawFloatControl("Field Of View", cameraProperties.FieldOfView, 45);
			lastControlActivated |= ImGuiUtil::IsControlActivated();
			lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
			lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
		}

		isValueChanged |= DrawFloatControl("Clip Distance | Near", cameraProperties.NearClipDistance, 0.1f);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
		isValueChanged |= DrawFloatControl("Clip Distance | Far", cameraProperties.FarClipDistance, 100);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

		ImGui::Separator();

		int index = cameraProperties.TargetWindowIndex;
		bool const indexChanged = DrawDropdownControl("Target Window Index", index, s_WindowIndexDropDownOptions);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
		ImGui::SameLine();
		ImGuiUtil::DrawHelpMarker("If there is no window with the given index, the camera will be skipped when rendering. "
								  "You can check how many windows there are in");
		if (indexChanged)
		{
			cameraProperties.TargetWindowIndex = index;
		}
		isValueChanged |= indexChanged;

		isValueChanged |= DrawBoolControl("Use Manual Aspect Ratio", cameraProperties.UseManualAspectRatio);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

		if (cameraProperties.UseManualAspectRatio)
		{
			isValueChanged |= DrawFloatControl("Manual Aspect Ratio", cameraProperties.ManualAspectRatio, 16.0f / 9.0f);
			lastControlActivated |= ImGuiUtil::IsControlActivated();
			lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
			lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
		}
		else
		{
			auto dimension = cameraProperties.GetTargetDimension();
			DrawReadOnlyTextWithLabel("Aspect Ratio", std::to_string(
				cameraProperties.GetAutomaticAspectRatioOfDimension(dimension)));
		}

		int32_t viewportType = static_cast<int32_t>(cameraProperties.ViewportValueType);
		bool viewportTypeChanged = DrawDropdownControl("Viewport Type", viewportType, {"Relative", "Absolute"});
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
		if (viewportTypeChanged)
		{
			cameraProperties.ViewportValueType = static_cast<ViewportValueType>(viewportType);
		}
		isValueChanged |= viewportTypeChanged;

		auto targetDimension = cameraProperties.GetTargetDimension();
		Math::Rect resetRect = {0, 0, 1, 1};
		if (cameraProperties.ViewportValueType == ViewportValueType::AbsoluteDimension)
		{
			resetRect = {0, 0, (float) targetDimension.x, (float) targetDimension.y};
		}
		isValueChanged |= DrawRectControl("Viewport", cameraProperties.Viewport, resetRect);
		lastControlActivated |= ImGuiUtil::IsControlActivated();
		lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
		lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

		s_Context.IsLastControlActivated = lastControlActivated;
		s_Context.IsLastControlDeactivated = lastControlDeactivated;
		s_Context.IsLastControlDeactivatedAfterEdit = lastControlDeactivatedAfterEdit;

		ImGui::PopID();

		return false;
	}

	bool DrawMaterialControl(const std::string& label, Material& material)
	{
		s_Context.Reset();
		// We use a local variable to keep track of it because this Control is a compound control.
		// We will set the value to s_Context.LastControlDeactivatedAfterEdit at the end.
		bool lastControlActivated = false;
		bool lastControlDeactivated = false;
		bool lastControlDeactivatedAfterEdit = false;

		bool isValueChanged = false;
		auto uiId = "##" + material.GetName();
		ImGui::Text(label.c_str());

		ShaderProgram const& shader = material.GetShaderProgram();
		auto const& allProperties = shader.GetAllPropertyInfo();

		for (auto& propertyInfo : allProperties)
		{
			bool isPropertyValueChanged = false;
			auto& propertyName = propertyInfo.UniformName;

			// TODO: Material imgui interface, add more type UI support
			float floatValue;
			glm::vec4 float4Value;
			switch (propertyInfo.Type)
			{
				case UniformType::Float:
					floatValue = material.GetFloat(propertyName);
					isPropertyValueChanged = DrawFloatControl(propertyInfo.DisplayName + uiId, floatValue, 0.0f);
					lastControlActivated |= ImGuiUtil::IsControlActivated();
					lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
					lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
					if (isPropertyValueChanged)
					{
						material.SetFloat(propertyName, floatValue);
					}
					break;
				case UniformType::Float2:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Float3:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Float4:
					float4Value = material.GetFloat4(propertyName);
					isPropertyValueChanged = DrawColor4Control(propertyInfo.DisplayName + uiId, float4Value);
					lastControlActivated |= ImGuiUtil::IsControlActivated();
					lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
					lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();
					if (isPropertyValueChanged)
					{
						material.SetFloat4(propertyName, float4Value);
					}
					break;
				case UniformType::Mat3:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Mat4:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Int:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Boolean:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Texture2D:
					ImGui::Text(propertyInfo.DisplayName.c_str());
					break;
				case UniformType::Invalid:
					break;
			}

			isValueChanged |= isPropertyValueChanged;
		}

		s_Context.IsLastControlActivated = lastControlActivated;
		s_Context.IsLastControlDeactivated = lastControlDeactivated;
		s_Context.IsLastControlDeactivatedAfterEdit = lastControlDeactivatedAfterEdit;

		return isValueChanged;
	}

	void DrawReadOnlyTextWithLabel(const std::string &label, const std::string &text)
	{
		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			ImGui::Text(text.c_str());
			ImGui::PopItemWidth();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void DrawTexture2DPreviewWithLabel(const std::string &label, const std::shared_ptr<Texture2D> &texture)
	{
		static bool isTexturePreviewWindowOpen = false;

		ImGuiIO &io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Settings::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			if (texture)
			{
				if (texture == Texture2D::GetDefaultTexture())
				{
					ImGui::Text("Default White Texture"); ImGui::SameLine();
					ImGuiUtil::DrawHelpMarker("When the given path is not a texture asset, Texture2D::GetDefaultTexture is used instead.");
				}
				else
				{
					auto imTexId = (void*)(intptr_t)(texture->GetID());
					const char* previewWindowPopupId = "TexturePreviewWindow";
					ImGui::Text("%dx%d", texture->GetWidth(), texture->GetHeight()); ImGui::Spacing();
					ImVec2 const textureSize = ImVec2(texture->GetWidth(), texture->GetHeight());
					ImVec2 const size = ImVec2(32.0f, 32.0f);
					ImVec2 const uv0 = ImVec2(0, 1); ImVec2 const uv1 = ImVec2(1, 0);
					ImVec4 const bgColor = ImVec4(0, 0, 0, 1);
					ImVec4 const borderColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
					if (ImGui::ImageButton("", imTexId, size, uv0, uv1, bgColor))
					{
						isTexturePreviewWindowOpen = true;
						float const previewWindowPadding = 48;
						ImVec2 const previewWindowSize = ImVec2(textureSize.x + previewWindowPadding, textureSize.y + previewWindowPadding);
						ImGui::SetNextWindowPos(ImGui::GetMousePos());
						ImGui::SetNextWindowSize(previewWindowSize);
						ImGui::SetNextWindowFocus();
					}

					if (isTexturePreviewWindowOpen)
					{
						if (ImGui::Begin("Texture Preview Window", &isTexturePreviewWindowOpen))
						{
							ImGui::Image(imTexId, textureSize, uv0, uv1, ImVec4(1, 1, 1, 1), borderColor);
						}
						ImGui::End();
					}

					bool const isImageButtonHovered = ImGui::IsItemHovered();
					if (isImageButtonHovered)
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted("Click to open a separate persistent texture preview window.");
						ImGui::Image(imTexId, textureSize, uv0, uv1, ImVec4(1, 1, 1, 1), borderColor);
						ImGui::EndTooltip();
					}
				}
			}
			else
			{
				ImGui::Text("Invalid Texture");
			}

			ImGui::PopItemWidth();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
	}

	void DrawHelpMarker(const char* description)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(description);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
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

		if (FileSystem::FileExists(initiallySelectedFilePath))
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
		ImGuiViewport const* windowViewport = ImGui::GetWindowViewport();

		ImVec2 const popupPosition = {windowViewport->GetWorkCenter().x - defaultPopupWindowSize.x / 2, windowViewport->GetWorkCenter().y - defaultPopupWindowSize.y / 2};
		ImGui::SetNextWindowPos(popupPosition, ImGuiCond_Appearing);

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
							if (params.IsSaveFilePanel && FileSystem::FileExists(outputPath))
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

		// Draw selected file path as text OR input field.
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

		// Draw the rest of the buttons & confirmSaveAs popup.
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
			bool const isSelectedFilePathValid = !FilePathPopup_SelectedFilePath.filename().empty();

			ImGui::BeginDisabled(!isSelectedFilePathValid);
			if (ImGui::Button("Confirm", buttonSize))
			{
				// For a save panel, we need to do some filename extension checking
				bool const hasAutoFileExtension = params.SaveFileExtension != nullptr;
				auto selectedFileExtension = FilePathPopup_SelectedFilePath.extension();
				if (hasAutoFileExtension && selectedFileExtension != params.SaveFileExtension)
				{
					// Append auto extension file if the filepath doesn't have a matching one.
					FilePathPopup_SelectedFilePath += params.SaveFileExtension;
				}
				// The final save directory depends on the current directory.
				outputPath = FilePathPopup_CurrentDirectory / FilePathPopup_SelectedFilePath.filename();

				if (FileSystem::FileExists(outputPath))
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
			ImGui::EndDisabled();

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
			bool const isSelectedFilePathValid = FileSystem::FileExists(FilePathPopup_SelectedFilePath);

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

	// !!!! INTERNAL FUNCTION SECTIONS !!!!
	void Internal::InteractableItem(const char *str_id, const ImVec2 &size_arg)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
		{
			return;
		}

		// Cannot use zero-size for InvisibleButton(). Unlike Button() there is not way to fallback using the label size.
		IM_ASSERT(size_arg.x != 0.0f && size_arg.y != 0.0f);

		const ImGuiID id = window->GetID(str_id);
		ImVec2 size = ImGui::CalcItemSize(size_arg, 0.0f, 0.0f);
		const ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y));
		ImGui::ItemSize(size);
		ImGui::ItemAdd(bb, id);
	}

	template<typename Type, typename ControlFunc>
	bool Internal::ArrayControl<Type, ControlFunc>::Draw()
	{
		auto &arrayLabel = Label;
		std::vector<Type> &elements = Elements;
		auto &controlFunction = ControlFunction;

		s_Context.Reset();
		// We use a local variable to keep track of it because this Control is a compound control.
		// We will set the value to s_Context.LastControlDeactivatedAfterEdit at the end.
		bool lastControlActivated = false;
		bool lastControlDeactivated = false;
		bool lastControlDeactivatedAfterEdit = false;

		bool changed = false;

		ImGui::PushID(arrayLabel.c_str());

		ImGuiTreeNodeFlags const treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap;
		if (ImGui::TreeNodeEx("Array Tree Node", treeNodeFlags, "%s (%d)", arrayLabel.c_str(), elements.size()))
		{
			char controlID[16];
			ImGuiUtil::Settings::ControlLabelWidth = 80;
			ImGui::Separator();
			int indexToRemove = -1;
			int indexToInsertNewElement = -1;
			struct MoveElement
			{
				int SrcIndex = -1;
				int DstIndex = -1;
			};
			MoveElement moveElement;

			for (int i = 0; i < elements.size(); i++)
			{
				sprintf(controlID, "Element %d", i);
				ImGui::PushID(controlID);

				ImVec2 const elementWidgetScreenPos = ImGui::GetCursorScreenPos();
				ImVec2 const elementWidgetSize = ImVec2(ImGui::GetContentRegionAvail().x,
														ImGui::GetFrameHeightWithSpacing());
				auto &element = elements[i];

				// Draw draggable target as invisible button.
				bool isBeingDragged = false;
				ImVec2 originalCursorPos = ImGui::GetCursorPos();

				float const dragHandleWidth = ImGuiUtil::Settings::ControlLabelWidth - 12;
				ImGui::InvisibleButton("ElementDragHandle",
									   ImVec2 {dragHandleWidth, ImGui::GetFrameHeightWithSpacing()});
				if (!ImGui::IsDragDropActive() && ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				}

				ImGuiDragDropFlags const dragDropFlags = ImGuiDragDropFlags_None;
				if (ImGui::BeginDragDropSource(dragDropFlags))
				{
					ImGui::SetDragDropPayload("ArrayIndex", &i, sizeof(int));

					// Preview the element control in the drag tooltip.
					auto const previewWindowSize = ImVec2 {elementWidgetSize.x,
														   ImGui::GetTextLineHeightWithSpacing()};
					ImGui::ItemSize(previewWindowSize);
					ImGui::SameLine();
					controlFunction(controlID, element);

					ImGui::EndDragDropSource();

					lastControlActivated = true;
					isBeingDragged = true;
				}
				ImGui::SetCursorPos(originalCursorPos);

				// If the element is being used a drag source, we make the control & buttons of this element transparent.
				float const originalAlpha = ImGui::GetStyle().Alpha;
				if (isBeingDragged)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3f);
				}

				changed |= controlFunction(controlID, element);
				lastControlActivated |= ImGuiUtil::IsControlActivated();
				lastControlDeactivated |= ImGuiUtil::IsControlDeactivated();
				lastControlDeactivatedAfterEdit |= ImGuiUtil::IsControlDeactivatedAfterEdit();

				ImGui::SameLine();
				if (ImGui::SmallButton("-"))
				{
					indexToRemove = i;
					changed = true;
					lastControlDeactivatedAfterEdit = true;
				}
				lastControlActivated |= ImGui::IsItemActivated();
				lastControlDeactivated |= ImGui::IsItemDeactivated();
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				{
					ImGui::BeginTooltip();
					ImGui::SetTooltip("Remove this element");
					ImGui::EndTooltip();
				}

				ImGui::SameLine();
				if (ImGui::SmallButton("+"))
				{
					indexToInsertNewElement = i + 1;
					changed = true;
					lastControlDeactivatedAfterEdit = true;
				}
				lastControlActivated |= ImGui::IsItemActivated();
				lastControlDeactivated |= ImGui::IsItemDeactivated();
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
				{
					ImGui::BeginTooltip();
					ImGui::SetTooltip("Add a new element under");
					ImGui::EndTooltip();
				}

				if (ImGui::IsDragDropActive())    // We only draw invisible drop handles when the user is dragging.
				{
					// Draw 2 droppable targets as invisible buttons.
					originalCursorPos = ImGui::GetCursorPos();

					auto const previewLineColor = ImGui::GetColorU32(ImGuiCol_DragDropTarget);
					float const previewLineHeadRadius = 4;

					ImGui::SetCursorScreenPos(elementWidgetScreenPos);
					Internal::InteractableItem("ElementDropHandle_Upper",
											   ImVec2 {elementWidgetSize.x, elementWidgetSize.y * 0.5f});
					if (ImGui::BeginDragDropTarget())
					{
						ImGuiPayload const *dropPayload = ImGui::AcceptDragDropPayload("ArrayIndex",
																					   ImGuiDragDropFlags_AcceptPeekOnly);
						if (dropPayload != nullptr)
						{
							DYE_ASSERT(dropPayload->DataSize == sizeof(int));
							int payloadIndex = *(const int *) dropPayload->Data;
							bool const isSource = payloadIndex == i;

							if (dropPayload->IsPreview() && !isSource)
							{
								ImVec2 const previewLineBegin = elementWidgetScreenPos;
								ImVec2 const previewLineEnd = ImVec2(previewLineBegin.x + elementWidgetSize.x,
																	 previewLineBegin.y);
								ImGui::GetWindowDrawList()->AddCircle(previewLineBegin, previewLineHeadRadius,
																	  previewLineColor, 4);
								ImGui::GetWindowDrawList()->AddLine(previewLineBegin, previewLineEnd, previewLineColor,
																	2);
							}

							if (dropPayload->IsDelivery() && !isSource)
							{
								moveElement.SrcIndex = payloadIndex;
								moveElement.DstIndex = i;
							}
						}
						ImGui::EndDragDropTarget();
					}

					Internal::InteractableItem("ElementDropHandle_Lower",
											   ImVec2 {elementWidgetSize.x, elementWidgetSize.y * 0.5f});
					if (ImGui::BeginDragDropTarget())
					{
						ImGuiPayload const *dropPayload = ImGui::AcceptDragDropPayload("ArrayIndex",
																					   ImGuiDragDropFlags_AcceptPeekOnly);
						if (dropPayload != nullptr)
						{
							DYE_ASSERT(dropPayload->DataSize == sizeof(int));
							int payloadIndex = *(const int *) dropPayload->Data;
							bool const isSource = payloadIndex == i;

							if (dropPayload->IsPreview() && !isSource)
							{
								ImVec2 const previewLineBegin = ImVec2(elementWidgetScreenPos.x,
																	   elementWidgetScreenPos.y + elementWidgetSize.y);
								ImVec2 const previewLineEnd = ImVec2(previewLineBegin.x + elementWidgetSize.x,
																	 previewLineBegin.y);
								ImGui::GetWindowDrawList()->AddCircle(previewLineBegin, previewLineHeadRadius,
																	  previewLineColor, 4);
								ImGui::GetWindowDrawList()->AddLine(previewLineBegin, previewLineEnd, previewLineColor,
																	2);
							}

							if (dropPayload->IsDelivery() && !isSource)
							{
								moveElement.SrcIndex = payloadIndex;
								moveElement.DstIndex = i + 1;
							}
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::SetCursorPos(originalCursorPos);
				}

				if (isBeingDragged)
				{
					ImGui::PopStyleVar();
				}
				ImGui::PopID();
			}

			if (indexToRemove != -1)
			{
				elements.erase(elements.begin() + indexToRemove);
			}
			else if (indexToInsertNewElement != -1)
			{
				// TODO: for now, we just insert with a default value using {}
				elements.insert(elements.begin() + indexToInsertNewElement, Type {});
			}
			// Notice here we also don't do anything if the destination is right after the source (i.e. SrcIndex + 1 != DstIndex),
			// that's because inserting an element 'after' the element is the same as not moving at all.
			else if (moveElement.SrcIndex != moveElement.DstIndex &&
					 moveElement.SrcIndex + 1 != moveElement.DstIndex)
			{

				Type temp = elements[moveElement.SrcIndex];
				elements.insert(elements.begin() + moveElement.DstIndex, temp);

				if (moveElement.SrcIndex < moveElement.DstIndex)
				{
					// The source index is smaller than the destination index,
					// we can safely erase with source index.
					elements.erase(elements.begin() + moveElement.SrcIndex);
				}
				else if (moveElement.SrcIndex > moveElement.DstIndex)
				{
					// The destination index is bigger than the destination index,
					// we need to increment the source index by 1.
					elements.erase(elements.begin() + moveElement.SrcIndex + 1);
				}

				changed = true;
				lastControlDeactivatedAfterEdit = true;
			}

			ImGuiUtil::Settings::ControlLabelWidth = ImGuiUtil::Settings::DefaultControlLabelWidth;

			ImGui::TreePop();
		}

		ImGui::PopID();

		s_Context.IsLastControlActivated = lastControlActivated;
		s_Context.IsLastControlDeactivated = lastControlDeactivated;
		s_Context.IsLastControlDeactivatedAfterEdit = lastControlDeactivatedAfterEdit;

		return changed;
	}

	template struct Internal::ArrayControl<DYE::GUID, bool (*)(const char *,::DYE::GUID &)>;
}