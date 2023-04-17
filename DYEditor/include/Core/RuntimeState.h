#pragma once

#include <vector>

namespace DYE::DYEditor
{
	enum class PlayModeStateChange
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
		virtual void OnPlayModeStateChanged(PlayModeStateChange stateChange) = 0;
	};

	struct RuntimeState
	{
		friend class SceneEditorLayer;
		friend class SceneRuntimeLayer;

	public:
		static bool IsPlaying();
		static bool IsEditor();
		static bool IsRuntime();

		/// This method will do nothing in runtime build because runtime build is always in play mode.
		static void SetIsPlaying(bool value);

		static void RegisterListener(RuntimeStateListenerBase *listener);
		static void UnregisterListener(RuntimeStateListenerBase *listenerToRemove);

	private:
		static void broadcastPlayModeStateChangedEvent(PlayModeStateChange stateChange);

	private:
		static std::vector<RuntimeStateListenerBase*> s_RuntimeStateListeners;
	};
}