#pragma once

#include "Math/Color.h"
#include "Graphics/Texture.h"

#include <memory>

#include <glm/glm.hpp>

namespace DYE::MiniGame
{
	struct Sprite
	{
		std::shared_ptr<Texture2D> Texture;
		glm::vec4 Color = Color::White;

		bool IsTiled = false;
		glm::vec2 TilingScale {1, 1};
		glm::vec2 TilingOffset {0, 0};
	};
}