#pragma once

#include "Math/AABB.h"

namespace DYE::Math
{
	bool AABBAABBIntersect2D(AABB const& a, AABB const& b)
	{
		// Two AABBs only overlap if they overlap on all axes.
		// Compare each dimension as if their extent along each dimension is an interval.
		if (a.Max[0] < b.Min[0] || a.Min[0] > b.Max[0]) return false;
		if (a.Max[1] < b.Min[1] || a.Min[1] > b.Max[1]) return false;

		return true;
	}

	bool AABBAABBIntersect(AABB const& a, AABB const& b)
	{
		// Two AABBs only overlap if they overlap on all axes.
		// Compare each dimension as if their extent along each dimension is an interval.
		if (a.Max[0] < b.Min[0] || a.Min[0] > b.Max[0]) return false;
		if (a.Max[1] < b.Min[1] || a.Min[1] > b.Max[1]) return false;
		if (a.Max[2] < b.Min[2] || a.Min[2] > b.Max[2]) return false;

		return true;
	}

	bool AABBSphereIntersect(AABB const& a, glm::vec3 center, float radius)
	{
		float const aabbToCenterSqrDistance = a.SqrDistance(center);
		return aabbToCenterSqrDistance <= radius * radius;
	}

	bool AABBCircleIntersect(AABB const& a, glm::vec2 center, float radius)
	{
		float const aabbToCenterSqrDistance = a.SqrDistance2D(center);
		return aabbToCenterSqrDistance <= radius * radius;
	}
}
