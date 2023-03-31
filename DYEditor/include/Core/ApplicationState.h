#pragma once

namespace DYE::DYEditor
{
	struct ApplicationState
	{
		friend class SceneEditorLayer;
		friend class SceneRuntimeLayer;

	public:
		static bool IsPlaying();
		static bool IsEditor();
		static bool IsRuntime();

	private:
		// TODO: use compile definition to make it const in runtime build.
		static bool s_IsPlaying;
		static const bool s_IsEditor;
		static const bool s_IsRuntime;
	};
}