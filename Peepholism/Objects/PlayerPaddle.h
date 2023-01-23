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

		float Speed = 3.0f;
		glm::vec2 MovementInputBuffer {0, 0};

		//Sprite Sprite;
	};
}