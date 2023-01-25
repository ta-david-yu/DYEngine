#pragma once

#include "Components/Transform.h"
#include "Components/Velocity.h"
#include "Components/Collider.h"
#include "Components/Sprite.h"
#include "Components/Hittable.h"

namespace DYE::MiniGame
{
	struct PongBall
	{
		Transform Transform;
		Velocity Velocity;
		CircleCollider Collider;
		Hittable Hittable;
		Sprite Sprite;
	};
}