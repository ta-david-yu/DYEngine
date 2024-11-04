#pragma once

#include <filesystem>

namespace DYE::DYEditor
{
    class Scene;

    struct RuntimeSceneManagement
    {
        friend class SceneRuntimeLayer;

    public:
        static Scene &GetActiveMainScene();

        /// This method would not immediately execute operations needed to load a new scene.
        /// It will be delayed to the end of the frame.
        static void LoadScene(std::filesystem::path const &sceneFilePath);

    private:
        static void executeSceneOperationIfAny();
    };
}