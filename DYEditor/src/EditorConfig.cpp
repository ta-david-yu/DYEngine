#include "EditorConfig.h"

#include "Util/Logger.h"
#include "Util/Macro.h"
#include "FileSystem/FileSystem.h"

#include <optional>
#include <fstream>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	enum class PathComponentType
	{
		First,
		Intermediate,
		Last
	};

	template<typename Func>
	void ForeachKeyPathComponent(std::string const &keyPath, Func func)
	{
		size_t start = 0;
		size_t pos = 0;

		bool isFirst = true;

		while ((pos = keyPath.find('.', start)) != std::string::npos)
		{
			std::string component = keyPath.substr(start, pos - start);

			// Execute function on each component.
			func(component, isFirst? PathComponentType::First : PathComponentType::Intermediate);
			isFirst = false;

			start = pos + 1;
		}

		std::string component = keyPath.substr(start);
		func(component, PathComponentType::Last);
	}

	struct EditorConfigData
	{
		bool IsLoaded = false;
		std::filesystem::path CurrentLoadedFilePath;

		std::ofstream FileStream;
		toml::table Table;
	};

	EditorConfigData s_Data;

	void EditorConfig::ResetToDefault()
	{
		if (!s_Data.IsLoaded)
		{
			return;
		}

		// Open the file and clean everything in it.
		s_Data.Table = toml::table();
		s_Data.FileStream.open(s_Data.CurrentLoadedFilePath, std::ios::trunc);

		// Set some default values here.
		// ...

		s_Data.FileStream << s_Data.Table;
		s_Data.FileStream.close();
	}

	void EditorConfig::LoadFromOrCreateDefaultAt(const std::filesystem::path &path)
	{
		if (s_Data.IsLoaded)
		{
			// Save the previous loaded config data to the file.
			s_Data.FileStream.open(s_Data.CurrentLoadedFilePath, std::ios::trunc);
			s_Data.FileStream << s_Data.Table;
			s_Data.FileStream.flush();
			s_Data.IsLoaded = false;
		}

		if (!FileSystem::FileExists(path))
		{
			// The file doesn't exist yet, create one with default values.
			s_Data.IsLoaded = true;
			s_Data.CurrentLoadedFilePath = path;
			ResetToDefault();
			return;
		}

		auto result = toml::parse_file(path.string());
		if (!result)
		{
			// Having an issue parsing config file.
			std::string errorDescription(result.error().description());
			DYE_LOG("Failed to parse editor configuration file at '%s'.\n"
					"Error: %s", path.string().c_str(), errorDescription.c_str());
			return;
		}

		s_Data.IsLoaded = true;
		s_Data.CurrentLoadedFilePath = path;
		s_Data.Table = std::move(result.table());
	}

	void EditorConfig::Save()
	{
		s_Data.FileStream.open(s_Data.CurrentLoadedFilePath, std::ios::trunc);
		s_Data.FileStream << s_Data.Table;
		s_Data.FileStream.flush();
	}

	template<typename T>
	T EditorConfig::GetOrDefault(const std::string &keyPath, T const &defaultValue)
	{
		if (!s_Data.IsLoaded)
		{
			DYE_LOG("Editor Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(LoadFromOrCreateDefaultAt));
			return defaultValue;
		}

		toml::node *node = s_Data.Table.get(keyPath);
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


	template bool EditorConfig::GetOrDefault<bool>(const std::string &key, bool const &defaultValue);
	template float EditorConfig::GetOrDefault<float>(const std::string &key, float const &defaultValue);
	template int EditorConfig::GetOrDefault<int>(const std::string &key, int const &defaultValue);
	template std::string EditorConfig::GetOrDefault<std::string>(const std::string &key, std::string const &defaultValue);
	template std::string_view EditorConfig::GetOrDefault<std::string_view>(const std::string &key, std::string_view const &defaultValue);

	template<typename T>
	void EditorConfig::SetAndSave(const std::string &keyPath, const T &value)
	{
		if (!s_Data.IsLoaded)
		{
			DYE_LOG("Editor Config hasn't been loaded. You might have forgot to call %s first.",
					NAME_OF(LoadFromOrCreateDefaultAt));
			return;
		}

		s_Data.Table.insert_or_assign(keyPath, value);
		s_Data.FileStream.open(s_Data.CurrentLoadedFilePath, std::ios::trunc);
		s_Data.FileStream << s_Data.Table;
		s_Data.FileStream.flush();
	}

	template void EditorConfig::SetAndSave<bool>(const std::string &key, const bool &value);
	template void EditorConfig::SetAndSave<float>(const std::string &key, const float &value);
	template void EditorConfig::SetAndSave<int>(const std::string &key, const int &value);
	template void EditorConfig::SetAndSave<std::string>(const std::string &key, const std::string &value);
	template void EditorConfig::SetAndSave<std::string_view>(const std::string &key, const std::string_view &value);

	template<typename T>
	void EditorConfig::Set(const std::string &keyPath, const T &value)
	{
		s_Data.Table.insert_or_assign(keyPath, value);
	}

	template void EditorConfig::Set<bool>(const std::string &key, const bool &value);
	template void EditorConfig::Set<float>(const std::string &key, const float &value);
	template void EditorConfig::Set<int>(const std::string &key, const int &value);
	template void EditorConfig::Set<std::string>(const std::string &key, const std::string &value);
	template void EditorConfig::Set<std::string_view>(const std::string &key, const std::string_view &value);
}