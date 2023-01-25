#pragma once

#include "Components/Transform.h"
#include "Components/Sprite.h"
#include "Components/Collider.h"

namespace DYE::MiniGame
{
	struct PlayerPaddle
	{
		int PlayerID;
		Transform Transform;
		BoxCollider Collider;

		float Speed = 7.0f;
		glm::vec2 MovementInputBuffer {0, 0};

		glm::vec2 VelocityBuffer {0, 0};
		inline Math::AABB GetAABB() const { return Math::AABB::CreateFromCenter(Transform.Position, Collider.Size); }
	};
}