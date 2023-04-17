#pragma once

#include <optional>
#include <filesystem>
#include <string>
#include <toml++/toml.h>

namespace DYE::DYEditor
{
	constexpr char const* DefaultEditorConfigFilePath = "editor.ini";
	constexpr char const* DefaultRuntimeConfigFilePath = "runtime.ini";

	struct ProjectConfig
	{
		static std::optional<ProjectConfig> TryLoadFromOrCreateDefaultAt(const std::filesystem::path &path);

		void InitializeAndSave();

		void Save();

		bool DrawConfigurationBrowserImGui(bool *pIsOpen);

		template<typename T>
		T GetOrDefault(std::string const &keyPath, T const &defaultValue);

		template<typename T>
		void SetAndSave(std::string const &keyPath, T const &value);

		template<typename T>
		void Set(std::string const &keyPath, T const &value);

	private:
		bool IsLoaded = false;
		std::filesystem::path CurrentLoadedFilePath;

		std::ofstream FileStream;
		// I hate that I have to include toml here, which is totally unnecessary if C++ was designed better.
		toml::table Table;
	};

	ProjectConfig& GetEditorConfig();
	ProjectConfig& GetRuntimeConfig();
}