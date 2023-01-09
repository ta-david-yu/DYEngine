#pragma once

#include "Math/AABB.h"

namespace DYE::Math
{
	bool AABBAABBIntersect2D(AABB const& a, AABB const& b);
	bool AABBAABBIntersect(AABB const& a, AABB const& b);

	bool AABBSphereIntersect(AABB const& a, glm::vec3 center, float radius);
	bool AABBCircleIntersect(AABB const& a, glm::vec2 center, float radius);
}
