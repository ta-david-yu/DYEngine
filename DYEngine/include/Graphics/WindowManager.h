#pragma once

#include "WindowBase.h"

#include <map>
#include <vector>
#include <memory>
#include <optional>

namespace DYE
{
	class WindowManager
	{
	private:
		static std::vector<std::pair<WindowID, std::unique_ptr<WindowBase>>> s_Windows;
		static std::optional<WindowID> s_MainWindowID;

	public:
		/// Unlike WindowBase::Create(), this function creates a window that is registered under WindowManager.
		/// It still first calls WindowBase::Create(). If the window created is the first window, it's set as the main window.
		/// Registered window could be acquired through GetWindowFromID(id).
		/// \param windowProperty
		/// \return a raw pointer to the new window
		static WindowBase* CreateWindow(WindowProperty const& windowProperty);
		static bool CloseWindow(WindowID id);
		static void SetMainWindow(WindowID id);
		static WindowBase* GetWindowFromID(WindowID id);
		static bool HasWindowWithID(WindowID id);
		static bool IsMainWindow(WindowBase const& window);
		static std::optional<WindowID> GetMainWindowID();
		static WindowBase* GetMainWindow();
		static std::uint32_t GetNumberOfWindows() { return s_Windows.size(); }
		static WindowBase* GetMouseFocusedWindow();
		static void UpdateWindows();

		static void DrawWindowManagerImGui();

	private:
		static void drawRegisteredWindowsInspectorImGui();
	};
}