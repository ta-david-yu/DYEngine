#include "ImGui/ImGuiUtil.h"

#include "Graphics/CameraProperties.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"
#include "Math/AABB.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>

namespace DYE::ImGuiUtil
{
	namespace Parameters
	{
		float ControlLabelWidth = 200.0f;

		void ResetParametersToDefaultValues()
		{
			ControlLabelWidth = DefaultControlLabelWidth;
		}
	}

	bool DrawVec2Control(const std::string& label, glm::vec2& value, float resetValue)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4 {0.9f, 0.2f, 0.2f, 1.0f});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4 {0.8f, 0.1f, 0.15f, 1.0f});
			ImGui::PushFont(boldFont);
			if (ImGui::Button("X", buttonSize))
			{
				value.x = resetValue;
				isValueChanged |= true;
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			isValueChanged |= ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
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
			}
			ImGui::PopFont();
			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			isValueChanged |= ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawVec3Control(const std::string& label, glm::vec3& value, float resetValue)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value.x = resetValue;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value.y = resetValue;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			value.z = resetValue;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawBooleanControl(const std::string& label, bool& value)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		isValueChanged |= ImGui::Checkbox("##Boolean", &value);

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawFloatControl(const std::string& label, float& value, float resetValue)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		// Drag Float
		{
			isValueChanged |= ImGui::DragFloat("##Float", &value, 0.1f, 0, 0, "%.2f");
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
			}
			ImGui::PopFont();
			///ImGui::PopStyleColor(3);
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}
	bool DrawUnsignedIntControl(const std::string& label, int32_t & value, int32_t resetValue)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		// Drag Float
		{
			isValueChanged |= ImGui::DragInt("##UnsignedInt", &value, 1, 0, 0);
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
			}
			ImGui::PopFont();
			///ImGui::PopStyleColor(3);
		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawColor4Control(const std::string& label, glm::vec4& value)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		isValueChanged |= ImGui::ColorEdit4("##Color", glm::value_ptr(value));

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawRectControl(const std::string& label, Math::Rect& value, Math::Rect const& resetValue)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			value.X = resetValue.X;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##X", &value.X, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			value.Y = resetValue.Y;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##Y", &value.Y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("W", buttonSize))
		{
			value.Width = resetValue.Width;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##W", &value.Width, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("H", buttonSize))
		{
			value.Height = resetValue.Height;
			isValueChanged |= true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		isValueChanged |= ImGui::DragFloat("##H", &value.Height, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawToolbar(const std::string& label, int32_t & value, std::vector<std::string> const& texts)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(texts.size(), ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			for (int i = 0; i < texts.size(); ++i)
			{
				isValueChanged |= ImGui::RadioButton(texts[i].c_str(), &value, i);
				if (i != texts.size() - 1)
				{
					ImGui::SameLine();
				}
			}
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();

		return isValueChanged;
	}

	bool DrawDropdown(const std::string& label, int32_t& value, std::vector<char const*> const& texts)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
		{
			texts.data();
			isValueChanged |= ImGui::Combo("##combo", &value, texts.data(), texts.size());

			ImGui::PopItemWidth();
		}
		ImGui::PopStyleVar();
		ImGui::Columns(1);
		ImGui::PopID();
		return isValueChanged;
	}

	void DrawReadOnlyTextWithLabel(const std::string &label, const std::string &text)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
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

	bool DrawAABBControl(const std::string& label, Math::AABB& aabb)
	{
		bool isValueChanged = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, Parameters::ControlLabelWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		{
			ImGui::PushMultiItemsWidths(4, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 {0, 0});
			{
				ImGui::LabelText("", "Min");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MinX", &aabb.Min.x, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MinY", &aabb.Min.y, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MinZ", &aabb.Min.z, 0.1f, 0.0f, 0.0f, "%.2f");
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
				isValueChanged |= ImGui::DragFloat("##MaxX", &aabb.Max.x, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MaxY", &aabb.Max.y, 0.1f, 0.0f, 0.0f, "%.2f");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				isValueChanged |= ImGui::DragFloat("##MaxZ", &aabb.Max.z, 0.1f, 0.0f, 0.0f, "%.2f");
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

	bool DrawCameraPropertiesControl(const std::string& label, CameraProperties& cameraProperties)
	{
		bool isValueChanged = false;
		ImGui::Text(label.c_str());

		isValueChanged |= DrawVec3Control("Position", cameraProperties.Position);
		isValueChanged |= DrawBooleanControl("Is Orthographic", cameraProperties.IsOrthographic);

		if (cameraProperties.IsOrthographic)
		{
			isValueChanged |= DrawFloatControl("Orthographic Size", cameraProperties.OrthographicSize, 1);
		}
		else
		{
			isValueChanged |= DrawFloatControl("Field Of View", cameraProperties.FieldOfView, 45);
		}

		isValueChanged |= DrawFloatControl("Clip Distance | Near", cameraProperties.NearClipDistance, 0.1f);
		isValueChanged |= DrawFloatControl("Clip Distance | Far", cameraProperties.FarClipDistance, 100);

		isValueChanged |= DrawBooleanControl("Use Manual Aspect Ratio", cameraProperties.UseManualAspectRatio);
		if (cameraProperties.UseManualAspectRatio)
		{
			isValueChanged |= DrawFloatControl("Manual Aspect Ratio", cameraProperties.ManualAspectRatio, 16.0f / 9.0f);
		}
		else
		{
			auto dimension = cameraProperties.GetTargetDimension();
			DrawReadOnlyTextWithLabel("Aspect Ratio", std::to_string(
				cameraProperties.GetAutomaticAspectRatioOfDimension(dimension)));
		}

		int32_t viewportType = static_cast<int32_t>(cameraProperties.ViewportValueType);
		bool viewportTypeChanged = DrawDropdown("Viewport Type", viewportType, { "Relative", "Absolute" });
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

		return false;
	}

	bool DrawMaterialControl(const std::string& label, Material& material)
	{
		bool isValueChanged = false;
		auto uiId = "##" + material.GetName();
		ImGui::Text(label.c_str());

		ShaderProgram const& shader = material.GetShaderProgram();
		auto const& allProperties = shader.GetAllPropertyInfo();

		for (auto& propertyInfo : allProperties)
		{
			bool isPropertyValueChanged = false;
			auto& propertyName = propertyInfo.UniformName;

			// TODO: Add more type UI support
			float floatValue;
			glm::vec4 float4Value;
			switch (propertyInfo.Type)
			{
				case UniformType::Float:
					floatValue = material.GetFloat(propertyName);
					isPropertyValueChanged = DrawFloatControl(propertyInfo.DisplayName + uiId, floatValue, 0.0f);
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

		return isValueChanged;
	}
}