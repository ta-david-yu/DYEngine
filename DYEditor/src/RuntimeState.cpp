#include "Core/RuntimeState.h"

#include <vector>

namespace DYE::DYEditor
{
	struct RuntimeStateData
	{
		// TODO: use compile definition to make it true by default in runtime build.
		bool IsPlaying = false;

		std::vector<RuntimeStateListenerBase*> RuntimeStateListeners = {};
	};

	static RuntimeStateData s_Data;

	bool RuntimeState::IsPlaying()
	{
		return s_Data.IsPlaying;
	}

	bool RuntimeState::IsEditor()
	{
		// TODO: use compile definition to return constant value based on whether it's a runtime OR editor build.
		return true;
	}

	bool RuntimeState::IsRuntime()
	{
		// TODO: use compile definition to return constant value based on whether it's a runtime OR editor build.
		return false;
	}

	void RuntimeState::SetIsPlaying(bool value)
	{
		bool const prevIsPlaying = s_Data.IsPlaying;
		bool const playModeChanged = value != prevIsPlaying;

		if (playModeChanged)
		{
			broadcastPlayModeStateChangedEvent(value ? PlayModeStateChange::BeforeEnterPlayMode : PlayModeStateChange::BeforeEnterEditMode);
		}

		// Change play mode.
		s_Data.IsPlaying = value;


		if (playModeChanged)
		{
			broadcastPlayModeStateChangedEvent(value ? PlayModeStateChange::AfterEnterPlayMode : PlayModeStateChange::AfterEnterEditMode);
		}
	}

	void RuntimeState::RegisterListener(RuntimeStateListenerBase *listener)
	{
		s_Data.RuntimeStateListeners.push_back(listener);
	}

	void RuntimeState::UnregisterListener(RuntimeStateListenerBase *listenerToRemove)
	{
		std::remove_if(s_Data.RuntimeStateListeners.begin(), s_Data.RuntimeStateListeners.end(),
					   [listenerToRemove](RuntimeStateListenerBase* listener)
					   {
						   return  listener == listenerToRemove;
					   });
	}

	void RuntimeState::broadcastPlayModeStateChangedEvent(PlayModeStateChange stateChange)
	{
		for (auto listener : s_Data.RuntimeStateListeners)
		{
			listener->OnPlayModeStateChanged(stateChange);
		}
	}
}

