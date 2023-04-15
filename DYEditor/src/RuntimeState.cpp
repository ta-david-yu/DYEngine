#include "Core/RuntimeState.h"

namespace DYE::DYEditor
{
	struct ApplicationStateData
	{
		// TODO: use compile definition to make it true by default in runtime build.
		bool IsPlaying = false;
	};

	static ApplicationStateData s_Data;

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
		s_Data.IsPlaying = value;
	}
}

