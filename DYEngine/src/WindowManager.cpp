#include "WindowManager.h"

#include "Util/Macro.h"

namespace DYE
{
	std::vector<std::pair<WindowID, std::unique_ptr<WindowBase>>> WindowManager::s_Windows = {};
	std::optional<WindowID> WindowManager::s_MainWindowID = {};

	WindowBase *WindowManager::CreateWindow(WindowProperty const& windowProperty)
	{
		auto window = WindowBase::Create(windowProperty);
		auto cachePtr = window.get(); 						// We cache the raw pointer because unique_ptr would be moved into vector later.
		WindowID const id = window->GetWindowID();
		s_Windows.emplace_back(id, std::move(window));

		if (!s_MainWindowID.has_value())
		{
			// If there is no main window yet, set the newly created window as main.
			s_MainWindowID = id;
		}

		return cachePtr;
	}

	void WindowManager::SetMainWindow(WindowID id)
	{
		s_MainWindowID = id;
	}

	WindowBase *WindowManager::GetWindowFromID(WindowID id)
	{
		for (auto const &windowPair: s_Windows)
		{
			if (windowPair.first != id)
			{
				continue;
			}

			return windowPair.second.get();
		}

		DYE_ASSERT_RELEASE(false && "There is no window with the given id!");
		return nullptr;

	}

	std::optional<WindowID> WindowManager::GetMainWindowID()
	{
		return s_MainWindowID;
	}

	WindowBase *WindowManager::GetMainWindow()
	{
		if (!s_MainWindowID.has_value())
		{
			DYE_ASSERT_RELEASE(false && "There is no main window!");
			return nullptr;
		}

		return GetWindowFromID(s_MainWindowID.value());
	}

	bool WindowManager::HasWindowWithID(WindowID id)
	{
		for (auto const &windowPair: s_Windows)
		{
			if (windowPair.first != id)
			{
				continue;
			}

			return true;
		}
		return false;
	}

	bool WindowManager::IsMainWindow(const WindowBase &window)
	{
		if (!s_MainWindowID.has_value())
		{
			return false;
		}

		return window.GetWindowID() == s_MainWindowID.value();
	}

	void WindowManager::UpdateWindows()
	{
		for (auto const &windowPair: s_Windows)
		{
			windowPair.second->OnUpdate();
		}
	}
}
