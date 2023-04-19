#pragma once

#include <toml++/toml.h>
#include <functional>
#include <vector>
#include "imgui.h"

namespace DYE::DYEditor
{
	struct EditorWindow
	{
		/// Name is also used as the key for storing EditorWindow data in config file.
		char const* Name;
		bool IsConfigOpen;
		std::function<void(char const *name, bool *pIsOpen, ImGuiViewport const* pMainViewportHint)> BodyFunction;
	};

	struct RegisterEditorWindowParameters
	{
		char const* Name;
		/// Whether or not the config isOpen value is true the first use.
		/// It's completely up to the user how they want to use the passed in pIsOpen parameter in the BodyFunction.
		bool isConfigOpenByDefault;
	};

	class EditorWindowManager
	{
	public:
		template<typename Func>
		static void RegisterEditorWindow(RegisterEditorWindowParameters parameters, Func func)
		{
			s_EditorWindows.emplace_back(
				EditorWindow
				{
					.Name = parameters.Name,
					.IsConfigOpen = parameters.isConfigOpenByDefault,
					.BodyFunction = func
				});
		}

		static void DrawEditorWindows(ImGuiViewport const *pMainViewportHint);

		template<typename Func>
		static void ForEachEditorWindow(Func func)
		{
			for (auto& editorWindow : s_EditorWindows)
			{
				func(editorWindow);
			}
		}

		inline static void ClearRegisteredEditorWindows() { s_EditorWindows.clear(); }
	private:
		static toml::table s_EditorWindowConfigFileTable;
		static std::vector<EditorWindow> s_EditorWindows;
	};
}