#include "Core/RuntimeSceneManagement.h"

#include "Core/RuntimeState.h"
#include "Core/Scene.h"
#include "Serialization/SerializedScene.h"
#include "Serialization/SerializedObjectFactory.h"
#include "Util/Logger.h"
#include "Util/Macro.h"

namespace DYE::DYEditor
{
    struct RuntimeSceneManagementData
    {
        Scene ActiveMainScene;

        // TODO: make a queue of operations instead of having one single flag to keep track of scene loading task.
        bool IsLoadingNewScene;
        std::optional<SerializedScene> SerializedSceneToLoad;
    };

    static RuntimeSceneManagementData s_Data;

    Scene &RuntimeSceneManagement::GetActiveMainScene()
    {
        return s_Data.ActiveMainScene;
    }

    void RuntimeSceneManagement::LoadScene(const std::filesystem::path &sceneFilePath)
    {
        DYE_ASSERT_LOG_WARN(RuntimeState::IsPlaying(), "You should not call RuntimeSceneManagement::LoadScene in Edit Mode.");

        s_Data.SerializedSceneToLoad = SerializedObjectFactory::TryLoadSerializedSceneFromFile(sceneFilePath);
        DYE_ASSERT_LOG_WARN(s_Data.SerializedSceneToLoad.has_value(), "Failed to load scene '%s'.", sceneFilePath.string().c_str());

        s_Data.IsLoadingNewScene = true;

        DYE_LOG("Load Scene: %s", sceneFilePath.string().c_str());
    }

    void RuntimeSceneManagement::executeSceneOperationIfAny()
    {
        if (!s_Data.IsLoadingNewScene)
        {
            return;
        }

        Scene &activeScene = GetActiveMainScene();

        // Execute teardown systems of the previous active scene & clear it.
        activeScene.ExecuteTeardownSystems();
        activeScene.Clear();

        SerializedObjectFactory::ApplySerializedSceneToEmptyScene(s_Data.SerializedSceneToLoad.value(), activeScene);

        // Initialize load systems.
        activeScene.ForEachSystemDescriptor
            (
                [&activeScene](SystemDescriptor &systemDescriptor, ExecutionPhase phase)
                {
                    systemDescriptor.Instance->InitializeLoad(
                        activeScene.World,
                        InitializeLoadParameters
                            {
                                .LoadType = InitializeLoadType::AfterLoadScene
                            });
                }
            );

        // Execute initialize systems.
        activeScene.ExecuteInitializeSystems();

        s_Data.IsLoadingNewScene = false;
    }
}