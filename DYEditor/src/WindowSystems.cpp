#include "Systems/WindowSystems.h"

#include "Core/Entity.h"
#include "Components/WindowComponents.h"
#include "Components/Command/WindowCommandComponents.h"

namespace DYE::DYEditor
{
	void CreateWindowOnInitializeSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		auto group = world.GetGroup<CreateWindowOnInitializeComponent>(Get<WindowHandleComponent>);
		for (auto entity : group)
		{
			// Create Window.
			auto createWindow = group.get<CreateWindowOnInitializeComponent>(entity);
			auto &windowHandle = group.get<WindowHandleComponent>(entity);

			WindowProperties properties
			(
				createWindow.Title,
				createWindow.InitialWidth,
				createWindow.InitialHeight,
				false	// FIXME: to let the user set isUserResizable as well
			);
			WindowBase *pNewWindow = WindowManager::CreateWindow(properties);

			if (createWindow.HasInitialPosition)
			{
				pNewWindow->SetPosition(createWindow.InitialPosition);
			}

			// Initialize WindowHandle with the newly created window info.
			windowHandle.IsCreated = true;
			windowHandle.ID = pNewWindow->GetWindowID();
			windowHandle.Index = WindowManager::TryGetWindowIndexFromID(windowHandle.ID).value();
		}
	}

	void ModifyWindowSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		// Set window position.
		{
			auto group = world.GetGroup<SetWindowPositionComponent>(Get<WindowHandleComponent>);
			for (auto entity : group)
			{
				auto setPosition = group.get<SetWindowPositionComponent>(entity);
				auto windowHandle = group.get<WindowHandleComponent>(entity);
				WindowBase *pWindow = windowHandle.TryGetWindow();
				if (pWindow == nullptr)
				{
					pWindow->SetPosition(setPosition.Position);
				}

				auto wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				wrappedEntity.RemoveComponent<SetWindowPositionComponent>();
			}
		}

		// Set window size.
		{
			auto group = world.GetGroup<SetWindowSizeComponent>(Get<WindowHandleComponent>);
			for (auto entity : group)
			{
				auto setSize = group.get<SetWindowSizeComponent>(entity);
				auto windowHandle = group.get<WindowHandleComponent>(entity);
				WindowBase *pWindow = windowHandle.TryGetWindow();
				if (pWindow == nullptr)
				{
					pWindow->SetSize(setSize.Size.x, setSize.Size.y);
				}

				auto wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				wrappedEntity.RemoveComponent<SetWindowSizeComponent>();
			}
		}

		// Set window title.
		{
			auto group = world.GetGroup<SetWindowTitleComponent>(Get<WindowHandleComponent>);
			for (auto entity : group)
			{
				auto setTitle = group.get<SetWindowTitleComponent>(entity);
				auto windowHandle = group.get<WindowHandleComponent>(entity);
				WindowBase *pWindow = windowHandle.TryGetWindow();
				if (pWindow == nullptr)
				{
					pWindow->SetTitle(setTitle.Title);
				}

				auto wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				wrappedEntity.RemoveComponent<SetWindowTitleComponent>();
			}
		}
	}

	void CloseWindowOnTearDownSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		auto view = world.GetView<WindowHandleComponent>();
		for (auto entity : view)
		{
			auto &windowHandle = view.get<WindowHandleComponent>(entity);
			if (windowHandle.IsCreated)
			{
				// Close window that was created.
				WindowManager::CloseWindow(windowHandle.ID);
			}
		}
	}
}