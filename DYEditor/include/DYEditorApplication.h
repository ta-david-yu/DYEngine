#pragma once

#include "Core/Application.h"

#include "SceneEditorLayer.h"
#include "SceneRuntimeLayer.h"

namespace DYE::DYEditor
{
	class DYEditorApplication final : public Application
	{
	public:
		DYEditorApplication() = delete;
		DYEditorApplication(const DYEditorApplication &) = delete;

		explicit DYEditorApplication(WindowProperties mainWindowProperties, int fixedFramePerSecond = 60);

		~DYEditorApplication() final = default;
	};
}