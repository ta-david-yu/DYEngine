#pragma once

#include "Components/Transform.h"
#include "Components/Sprite.h"
#include "Components/Collider.h"

namespace DYE::Pong
{
	struct Wall
	{
		Transform Transform;
		BoxCollider Collider;
	};
}