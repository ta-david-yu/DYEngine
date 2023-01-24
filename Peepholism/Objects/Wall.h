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

		inline Math::AABB GetAABB() const { return Math::AABB::CreateFromCenter(Transform.Position, Collider.Size); }
	};
}