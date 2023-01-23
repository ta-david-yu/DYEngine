#pragma once

#include "Math/Color.h"
#include "Graphics/Texture.h"

#include <memory>

#include <glm/glm.hpp>

namespace DYE::Pong
{
	struct Sprite
	{
		std::shared_ptr<Texture2D> Texture;
		glm::vec4 Color = Color::White;
	};
}