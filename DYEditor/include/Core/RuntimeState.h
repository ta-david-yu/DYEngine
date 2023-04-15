#pragma once

namespace DYE::DYEditor
{
	struct RuntimeState
	{
		friend class SceneEditorLayer;
		friend class SceneRuntimeLayer;

	public:
		static bool IsPlaying();
		static bool IsEditor();
		static bool IsRuntime();

		/// This method will do nothing in runtime build.
		static void SetIsPlaying(bool value);
	};
}