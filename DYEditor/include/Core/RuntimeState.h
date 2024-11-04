#pragma once

namespace DYE::DYEditor
{
    enum class ModeStateChange
    {
        // RuntimeState::IsPlaying() will be false.
        BeforeEnterPlayMode,

        // RuntimeState::IsPlaying() will be true.
        AfterEnterPlayMode,

        // RuntimeState::IsPlaying() will be true.
        BeforeEnterEditMode,

        // RuntimeState::IsPlaying() will be false.
        AfterEnterEditMode
    };

    struct RuntimeStateListenerBase
    {
        virtual void OnPlayModeStateChanged(ModeStateChange stateChange) = 0;
    };

    struct RuntimeState
    {
        friend class SceneEditorLayer;

        friend class SceneRuntimeLayer;

    public:
        static bool IsPlaying();
        static bool WillChangeMode();
        static bool IsEditor();
        static bool IsRuntime();

        /// This method will do nothing in runtime build because runtime build is always in play mode.\n
        /// The actual set result is delayed until the end of the frame by SceneRuntimeLayer.\n
        /// This means IsPlaying() would not reflect the set value right after SetIsPlayingAtTheEndOfFrame(), but you could use WillChangeMode() to
        /// check whether or not the editor is entering Play Mode soon.
        static void SetIsPlayingAtTheEndOfFrame(bool value);

        static void RegisterListener(RuntimeStateListenerBase *listener);
        static void UnregisterListener(RuntimeStateListenerBase *listenerToRemove);

    private:
        /// This method will do nothing in runtime build because runtime build is always in play mode, no way to change mode.\n
        static void consumeWillChangeModeIfNeeded();
        static void broadcastModeStateChangedEvent(ModeStateChange stateChange);
    };
}