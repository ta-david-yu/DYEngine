#pragma once

#include <optional>
#include <functional>
#include <filesystem>
#include <string>
#include <toml++/toml.h>
#include <glm/glm.hpp>

namespace DYE::DYEditor
{
    constexpr char const *DefaultEditorConfigFilePath = "settings//editor.ini";
    constexpr char const *DefaultRuntimeConfigFilePath = "settings//runtime.ini";

    namespace RuntimeConfigKeys
    {
        constexpr char const *ProjectName = "Project.Name";
        constexpr char const *FirstScene = "Project.FirstScene";
        constexpr char const *MainWindowWidth = "Window.Main.Width";
        constexpr char const *MainWindowHeight = "Window.Main.Height";
        constexpr char const *MainWindowFullScreenMode = "Window.Main.FullScreenMode";
        constexpr char const *SubWindows = "Window.SubWindows";
    }

    namespace EditorConfigKeys
    {
        constexpr char const *DefaultScene = "Editor.DefaultScene";
        constexpr char const *DebugInspector = "Editor.Inspector.DebugMode";
        constexpr char const *ShowSubWindowsInEditMode = "Editor.Inspector.ShowSubWindowsInEditMode";

        constexpr char const *SceneViewCameraPosition = "Editor.SceneView.Camera.Position";
        constexpr char const *SceneViewCameraClearColor = "Editor.SceneView.Camera.ClearColor";
    }

    struct ProjectConfig
    {
        static std::optional<ProjectConfig> TryLoadFromOrCreateDefaultAt(const std::filesystem::path &path);

        void InitializeAndSave();

        void Save();

        bool DrawGenericConfigurationBrowserImGui(char const *title, bool *pIsOpen);

        template<typename T>
        T GetOrDefault(std::string const &keyPath, T const &defaultValue);

        template<typename T>
        void SetAndSave(std::string const &keyPath, T const &value);

        template<typename T>
        void Set(std::string const &keyPath, T const &value);

        toml::table &Table() { return m_Table; }

    private:
        bool m_IsLoaded = false;
        std::filesystem::path m_CurrentLoadedFilePath;

        std::ofstream m_FileStream;
        // I hate that I have to include toml here, which is totally unnecessary if C++ was designed better.
        toml::table m_Table;
    };

    template<>
    glm::vec3 ProjectConfig::GetOrDefault<glm::vec3>(const std::string &keyPath, glm::vec3 const &defaultValue);
    template<>
    glm::vec4 ProjectConfig::GetOrDefault<glm::vec4>(const std::string &keyPath, glm::vec4 const &defaultValue);
    template<>
    void ProjectConfig::SetAndSave(const std::string &key, glm::vec3 const &value);
    template<>
    void ProjectConfig::SetAndSave(const std::string &key, glm::vec4 const &value);
    template<>
    void ProjectConfig::Set(const std::string &key, glm::vec3 const &value);
    template<>
    void ProjectConfig::Set(const std::string &key, glm::vec4 const &value);

    ProjectConfig &GetEditorConfig();
    ProjectConfig &GetRuntimeConfig();

    // FIXME: move this function to a separate file that stores all the important editor window calls.
    bool DrawEditorConfigurationWindow(bool *pIsOpen);

    // FIXME: move this function to a separate file that stores all the important editor window calls.
    bool DrawRuntimeConfigurationWindow(bool *pIsOpen);
}