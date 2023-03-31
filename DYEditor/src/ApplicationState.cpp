#include "Core/ApplicationState.h"

namespace DYE::DYEditor
{
	// TODO: use compile definition to make it const in runtime build
	bool ApplicationState::s_IsPlaying = false;

	// TODO: use compile definition to assign constant value based on whether it's a runtime OR editor build
	bool const ApplicationState::s_IsEditor = true;

	// TODO: use compile definition to assign constant value based on whether it's a runtime OR editor build
	bool const ApplicationState::s_IsRuntime = false;

	bool ApplicationState::IsPlaying()
	{
		return s_IsPlaying;
	}

	bool ApplicationState::IsEditor()
	{
		return s_IsEditor;
	}

	bool ApplicationState::IsRuntime()
	{
		return s_IsRuntime;
	}
}

