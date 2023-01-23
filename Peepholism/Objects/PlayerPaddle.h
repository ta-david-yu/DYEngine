#pragma once

#include "Components/Transform.h"
#include "Components/Sprite.h"
#include "Components/Collider.h"

namespace DYE::Pong
{
	struct PlayerPaddle
	{
		Transform Transform;
		BoxCollider Collider;

		float Speed = 5.0f;
		glm::vec2 MovementInputBuffer {0, 0};

		Math::AABB GetAABB() const { return Math::AABB::CreateFromCenter(Transform.Position, Collider.Size); }

		//Sprite Sprite;
	};
}