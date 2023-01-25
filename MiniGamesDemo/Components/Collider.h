#pragma once

#include "ColliderManager.h"

#include <glm/glm.hpp>

#include <optional>

namespace DYE::MiniGame
{
	struct BoxCollider
	{
		std::optional<ColliderID> ID;
		glm::vec3 Size {1, 1, 1};
	};

	struct CircleCollider
	{
		float Radius = 0.5f;
	};
}