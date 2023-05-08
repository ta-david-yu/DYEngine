#include "Core/RuntimeState.h"

#include <algorithm>
#include <vector>

namespace DYE::DYEditor
{
	struct RuntimeStateData
	{
		// TODO: use compile definition to make it true by default in runtime build.
#ifdef DYE_EDITOR
		bool IsPlaying = false;
#else
		static constexpr const bool IsPlaying = true;
#endif
		bool IsChangingMode = false;
		bool NewIsPlaying = false;
		std::vector<RuntimeStateListenerBase*> RuntimeStateListeners = {};
	};

	static RuntimeStateData s_Data;

	bool RuntimeState::IsPlaying()
	{
		return s_Data.IsPlaying;
	}

	bool RuntimeState::WillChangeMode()
	{
		return s_Data.IsChangingMode;
	}

	bool RuntimeState::IsEditor()
	{
		// TODO: use compile definition to return constant value based on whether it's a runtime OR editor build.
#ifdef DYE_EDITOR
		return true;
#else
		return false;
#endif
	}

	bool RuntimeState::IsRuntime()
	{
		// TODO: use compile definition to return constant value based on whether it's a runtime OR editor build.
#ifdef DYE_EDITOR
		return false;
#else
		return true;
#endif
	}

	void RuntimeState::SetIsPlayingAtTheEndOfFrame(bool value)
	{
#ifdef DYE_EDITOR
		if (s_Data.IsPlaying == value)
		{
			// The new value is the same as before, skip it.
			return;
		}

		s_Data.IsChangingMode = true;
		s_Data.NewIsPlaying = value;

		return;
#endif
	}

	void RuntimeState::RegisterListener(RuntimeStateListenerBase *listener)
	{
		s_Data.RuntimeStateListeners.push_back(listener);
	}

	void RuntimeState::UnregisterListener(RuntimeStateListenerBase *listenerToRemove)
	{
		std::erase_if(s_Data.RuntimeStateListeners,
					   [listenerToRemove](RuntimeStateListenerBase* listener)
					   {
						   return  listener == listenerToRemove;
					   });
	}

	void RuntimeState::consumeWillChangeModeIfNeeded()
	{
#ifdef DYE_EDITOR
		if (!s_Data.IsChangingMode)
		{
			return;
		}

		s_Data.IsChangingMode = false;
		bool value = s_Data.NewIsPlaying;

		bool const prevIsPlaying = s_Data.IsPlaying;
		bool const playModeChanged = value != prevIsPlaying;

		if (playModeChanged)
		{
			broadcastModeStateChangedEvent(
				value ? ModeStateChange::BeforeEnterPlayMode : ModeStateChange::BeforeEnterEditMode);
		}

		// Change play mode.
		s_Data.IsPlaying = value;

		if (playModeChanged)
		{
			broadcastModeStateChangedEvent(
				value ? ModeStateChange::AfterEnterPlayMode : ModeStateChange::AfterEnterEditMode);
		}
#endif
	}

	void RuntimeState::broadcastModeStateChangedEvent(ModeStateChange stateChange)
	{
		for (auto listener : s_Data.RuntimeStateListeners)
		{
			listener->OnPlayModeStateChanged(stateChange);
		}
	}
}

