#pragma once

#include "Components/Transform.h"
#include "Components/Velocity.h"
#include "Components/Collider.h"
#include "Components/Sprite.h"

namespace DYE::Pong
{
	struct Ball
	{
		Transform Transform;
		Velocity Velocity;
		CircleCollider Collider;
		Sprite Sprite;
	};
}