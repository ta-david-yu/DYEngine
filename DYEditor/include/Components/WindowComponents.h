#pragma once

#include "Graphics/WindowManager.h"
#include "Core/EditorComponent.h"

#include <glm/glm.hpp>

namespace DYE::DYEditor
{
	struct WindowHandleComponent
	{
		bool IsCreated = false;
		std::uint32_t Index = 0;
		WindowID ID = 0;

		[[nodiscard]]
		WindowBase *TryGetWindow() const
		{
			if (!IsCreated)
			{
				return nullptr;
			}
			return WindowManager::TryGetWindowAt(Index);
		}
	};

	struct CreateWindowOnInitializeComponent
	{
		bool HasInitialPosition = false;

		DYE::Int32 InitialWidth = 1600;
		DYE::Int32 InitialHeight = 900;
		DYE::Vector2 InitialPosition = {0, 0};
		std::string Title = "New Window";
	};
}