#pragma once

#include "Components/Transform.h"
#include "Components/Velocity.h"
#include "Components/Collider.h"
#include "Components/Sprite.h"
#include "Components/Hittable.h"
#include "Components/AttachableToPaddle.h"

namespace DYE::MiniGame
{
	struct PongBall
	{
		Transform Transform;
		Velocity Velocity;
		CircleCollider Collider;
		Hittable Hittable;
		AttachableToPaddle Attachable;
		Sprite Sprite;
	};
}