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
		Sprite Sprite;
		BoxCollider Collider;

		float Speed = 8.0f;
		float HorizontalBallSpeedIncreasePerHit = 1.0f;
		float MaxPositionY = 4.5f;
		float MinPositionY = -4.5;

		glm::vec2 MovementInputBuffer {0, 0};
		glm::vec2 VelocityBuffer {0, 0};
		inline Math::AABB GetAABB() const { return Math::AABB::CreateFromCenter(Transform.Position, Collider.Size); }
	};
}