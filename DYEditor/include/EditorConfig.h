#pragma once

#include <filesystem>
#include <string>

namespace DYE::DYEditor
{
	struct EditorConfig
	{
		static constexpr char const* DefaultEditorConfigFilePath = "editor.ini";

		static void ResetToDefault();

		static void LoadFromOrCreateDefaultAt(const std::filesystem::path &path);
		static void Save();

		template<typename T>
		static T GetOrDefault(std::string const &keyPath, T const &defaultValue);

		template<typename T>
		static void SetAndSave(std::string const &keyPath, T const &value);

		template<typename T>
		static void Set(std::string const &keyPath, T const &value);
	};
}