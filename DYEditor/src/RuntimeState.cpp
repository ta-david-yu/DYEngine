#include "Core/RuntimeState.h"

namespace DYE::DYEditor
{
	struct ApplicationStateData
	{
		// TODO: use compile definition to make it true by default in runtime build.
		bool IsPlaying = false;
	};

	static ApplicationStateData s_Data;

	std::vector<RuntimeStateListenerBase*> RuntimeState::s_RuntimeStateListeners = {};

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
		s_RuntimeStateListeners.push_back(listener);
	}

	void RuntimeState::UnregisterListener(RuntimeStateListenerBase *listenerToRemove)
	{
		std::remove_if(s_RuntimeStateListeners.begin(), s_RuntimeStateListeners.end(),
					   [listenerToRemove](RuntimeStateListenerBase* listener)
					   {
						   return  listener == listenerToRemove;
					   });
	}

	void RuntimeState::broadcastPlayModeStateChangedEvent(PlayModeStateChange stateChange)
	{
		for (auto listener : s_RuntimeStateListeners)
		{
			listener->OnPlayModeStateChanged(stateChange);
		}
	}
}

