#pragma once

#include "Components/Transform.h"
#include "Components/Collider.h"

namespace DYE::MiniGame
{
	struct PongGoalArea
	{
		int PlayerID = 0;
		Transform Transform;
		BoxCollider Collider;

		inline Math::AABB GetAABB() const { return Math::AABB::CreateFromCenter(Transform.Position, Collider.Size); }
	};
}
