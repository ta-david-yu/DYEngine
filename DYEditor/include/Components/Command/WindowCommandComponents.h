#pragma once

#include "Graphics/WindowManager.h"
#include "Core/EditorComponent.h"

#include <glm/glm.hpp>

namespace DYE::DYEditor
{
	struct SetWindowPositionComponent
	{
		glm::vec<2, std::int32_t> Position;
	};

	struct SetWindowSizeComponent
	{
		glm::vec<2, std::uint32_t> Size;
	};

	struct SetWindowTitleComponent
	{
		std::string Title;
	};
}