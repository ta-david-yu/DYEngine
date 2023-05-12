#include "ProjectConfig.h"

#include "Util/Logger.h"
#include "Util/Macro.h"
#include "FileSystem/FileSystem.h"
#include "ImGui/ImGuiUtil.h"

#include <optional>
#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	struct EditorConfigData
	{
		bool IsLoaded = false;
		std::filesystem::path CurrentLoadedFilePath;

		std::ofstream FileStream;
		toml::table Table;
	};

	std::optional<ProjectConfig> ProjectConfig::TryLoadFromOrCreateDefaultAt(const std::filesystem::path &path)
	{
		ProjectConfig config;
		if (!FileSystem::FileExists(path))
		{
			std::filesystem::create_directories(path.parent_path());
			std::filesystem::permissions(path.parent_path(), std::filesystem::perms::all);

			// The file doesn't exist yet, create one with default values.
			config.m_IsLoaded = true;
			config.m_CurrentLoadedFilePath = path;
			config.InitializeAndSave();
			return std::move(config);
		}

		auto result = toml::parse_file(path.string());
		if (!result)
		{
			// Having an issue parsing config file.
			std::string errorDescription(result.error().description());
			DYE_LOG("Failed to parse editor configuration file at '%s'.\n"
					"Error: %s", path.string().c_str(), errorDescription.c_str());
			return {};
		}

		config.m_IsLoaded = true;
		config.m_CurrentLoadedFilePath = path;
		config.m_Table = std::move(result.table());
		return std::move(config);
	}

	void ProjectConfig::InitializeAndSave()
	{
		if (!m_IsLoaded)
		{
			return;
		}

		// Open the file and clean everything in it.
		m_Table = toml::table();
		m_FileStream.open(m_CurrentLoadedFilePath, std::ios::trunc);
		m_FileStream.close();
	}

	void ProjectConfig::Save()
	{
		m_FileStream.open(m_CurrentLoadedFilePath, std::ios::trunc);
		m_FileStream << m_Table;
		m_FileStream.close();
	}

	/// \return true if the configuration has been changed. Otherwise false.
	bool ProjectConfig::DrawGenericConfigurationBrowserImGui(char const* title, bool *pIsOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(title, pIsOpen))
		{
			ImGui::End();
			return false;
		}

		ImGui::Separator();
		bool changed = false;

		ImGui::PushID(m_CurrentLoadedFilePath.string().c_str());
		for (auto iterator = m_Table.begin(); iterator != m_Table.end(); iterator++)
		{
			std::string key(iterator->first.str());
			auto &value = iterator->second;
			if (value.is_boolean())
			{
				changed |= ImGuiUtil::DrawBoolControl(key, value.as_boolean()->get());
			}
			else if (value.is_floating_point())
			{
				// We need to do this extra double -> float conversion because toml++ stores floating point as double.
				float tempFloat = value.as_floating_point()->get();
				if (ImGuiUtil::DrawFloatControl(key, tempFloat))
				{
					changed = true;
					value.as_floating_point()->get() = tempFloat;
				}
			}
			else if (value.is_integer())
			{
				// We need to do this extra double -> float conversion because toml++ stores floating point as double.
				int tempInt = value.as_integer()->get();
				if (ImGuiUtil::DrawIntControl(key, tempInt))
				{
					changed = true;
					value.as_integer()->get() = tempInt;
				}
			}
			else if (value.is_string())
			{
				changed |= ImGuiUtil::DrawTextControl(key, value.as_string()->get());
			}
			else
			{
				// TODO: Not-yet supported type. Kinda lazy to implement error handling now :P
			}
		}

		ImGui::Separator();
		if (ImGui::Button("Save"))
		{
			Save();
		}

		ImGui::PopID();
		ImGui::End();
		return changed;
	}

	template<typename T>
	T ProjectConfig::GetOrDefault(const std::string &keyPath, T const &defaultValue)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return defaultValue;
		}

		toml::node *node = m_Table.get(keyPath);
		if (node == nullptr)
		{
			return defaultValue;
		}

		std::optional<T> result = node->value<T>();
		if (!result.has_value())
		{
			return defaultValue;
		}

		return result.value();
	}

	template bool ProjectConfig::GetOrDefault<bool>(const std::string &key, bool const &defaultValue);
	template float ProjectConfig::GetOrDefault<float>(const std::string &key, float const &defaultValue);
	template int ProjectConfig::GetOrDefault<int>(const std::string &key, int const &defaultValue);
	template std::string ProjectConfig::GetOrDefault<std::string>(const std::string &key, std::string const &defaultValue);
	template<>
	glm::vec3 ProjectConfig::GetOrDefault<glm::vec3>(const std::string &keyPath, glm::vec3 const &defaultValue)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return defaultValue;
		}

		toml::node const *pNode = m_Table.get(keyPath);
		if (pNode == nullptr)
		{
			return defaultValue;
		}

		toml::table const *pTable = pNode->as_table();
		auto x = pTable->get("x")->value_or<float>(0);
		auto y = pTable->get("y")->value_or<float>(0);
		auto z = pTable->get("z")->value_or<float>(0);

		return glm::vec3 {x, y, z};
	}
	template<>
	glm::vec4 ProjectConfig::GetOrDefault<glm::vec4>(const std::string &keyPath, glm::vec4 const &defaultValue)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return defaultValue;
		}

		toml::node const *pNode = m_Table.get(keyPath);
		if (pNode == nullptr)
		{
			return defaultValue;
		}

		toml::table const *pTable = pNode->as_table();
		auto x = pTable->get("x")->value_or<float>(0);
		auto y = pTable->get("y")->value_or<float>(0);
		auto z = pTable->get("z")->value_or<float>(0);
		auto w = pTable->get("w")->value_or<float>(0);

		return glm::vec4 {x, y, z, w};
	}

	template<typename T>
	void ProjectConfig::SetAndSave(const std::string &keyPath, const T &value)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return;
		}

		m_Table.insert_or_assign(keyPath, value);
		m_FileStream.open(m_CurrentLoadedFilePath, std::ios::trunc);
		m_FileStream << m_Table;
		m_FileStream.flush();
	}

	template void ProjectConfig::SetAndSave<bool>(const std::string &key, const bool &value);
	template void ProjectConfig::SetAndSave<float>(const std::string &key, const float &value);
	template void ProjectConfig::SetAndSave<int>(const std::string &key, const int &value);
	template void ProjectConfig::SetAndSave<std::string>(const std::string &key, const std::string &value);
	template<>
	void ProjectConfig::SetAndSave(const std::string &key, glm::vec3 const &value)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return;
		}

		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z} };
		table.is_inline(true);
		m_Table.insert_or_assign(key, std::move(table));
		m_FileStream.open(m_CurrentLoadedFilePath, std::ios::trunc);
		m_FileStream << m_Table;
		m_FileStream.flush();
	}
	template<>
	void ProjectConfig::SetAndSave(const std::string &key, glm::vec4 const &value)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return;
		}

		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z}, {"w", value.w} };
		table.is_inline(true);
		m_Table.insert_or_assign(key, std::move(table));
		m_FileStream.open(m_CurrentLoadedFilePath, std::ios::trunc);
		m_FileStream << m_Table;
		m_FileStream.flush();
	}

	template<typename T>
	void ProjectConfig::Set(const std::string &keyPath, const T &value)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return;
		}

		m_Table.insert_or_assign(keyPath, value);
	}

	template void ProjectConfig::Set<bool>(const std::string &key, const bool &value);
	template void ProjectConfig::Set<float>(const std::string &key, const float &value);
	template void ProjectConfig::Set<int>(const std::string &key, const int &value);
	template void ProjectConfig::Set<std::string>(const std::string &key, const std::string &value);
	template<>
	void ProjectConfig::Set(const std::string &key, glm::vec3 const &value)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return;
		}

		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z} };
		table.is_inline(true);
		m_Table.insert_or_assign(key, std::move(table));
	}
	template<>
	void ProjectConfig::Set(const std::string &key, glm::vec4 const &value)
	{
		if (!m_IsLoaded)
		{
			DYE_LOG("Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(TryLoadFromOrCreateDefaultAt));
			return;
		}

		toml::table table { {"x", value.x}, {"y", value.y}, {"z", value.z}, {"w", value.w} };
		table.is_inline(true);
		m_Table.insert_or_assign(key, std::move(table));
	}

	ProjectConfig& GetEditorConfig()
	{
		static ProjectConfig config = ProjectConfig::TryLoadFromOrCreateDefaultAt(DefaultEditorConfigFilePath).value();
		return config;
	}

	ProjectConfig& GetRuntimeConfig()
	{
		static ProjectConfig config = ProjectConfig::TryLoadFromOrCreateDefaultAt(DefaultRuntimeConfigFilePath).value();
		return config;
	}

	bool DrawRuntimeConfigurationWindow(bool *pIsOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Runtime Configuration", pIsOpen))
		{
			ImGui::End();
			return false;
		}

		ImGui::Separator();
		ProjectConfig& config = GetRuntimeConfig();

		bool changed = false;
		ImGui::PushID("Runtime Configuration");

		auto projectName = config.GetOrDefault<std::string>(RuntimeConfigKeys::ProjectName, "Sandbox");
		if (ImGuiUtil::DrawTextControl("Project Name", projectName))
		{
			config.Set(RuntimeConfigKeys::ProjectName, projectName);
			changed = true;
		}

		auto firstScenePath = (std::filesystem::path) config.GetOrDefault<std::string>(RuntimeConfigKeys::FirstScene, "");
		if (ImGuiUtil::DrawAssetPathStringControl("First Scene", firstScenePath, { ".tscene" }))
		{
			config.Set(RuntimeConfigKeys::FirstScene, firstScenePath.string());
			changed = true;
		}

		auto mainWindowWidth = config.GetOrDefault<int>(RuntimeConfigKeys::MainWindowWidth, 1600);
		if (ImGuiUtil::DrawIntControl("Main Window Width", mainWindowWidth, 1600))
		{
			config.Set(RuntimeConfigKeys::MainWindowWidth, mainWindowWidth);
			changed = true;
		}

		auto mainWindowHeight = config.GetOrDefault<int>(RuntimeConfigKeys::MainWindowHeight, 900);
		if (ImGuiUtil::DrawIntControl("Main Window Height", mainWindowHeight, 900))
		{
			config.Set(RuntimeConfigKeys::MainWindowHeight, mainWindowHeight);
			changed = true;
		}

		ImGui::Separator();

		if (ImGui::Button("Save"))
		{
			config.Save();
		}

		ImGui::PopID();
		ImGui::End();

		return changed;
	}
}