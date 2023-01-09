#include "Math/PrimitiveTest.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>

#include <climits>

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

	bool RayAABBIntersect2D(glm::vec2 rayOrigin, glm::vec2 rayDirection, Math::AABB const& aabb, float& hitTime, glm::vec2& hitPoint)
	{
		hitTime = 0.0f; // Could also call it slab 'enter time'. We could set this to -float.max to get hit on the line instead of the ray.
		float exitTime = std::numeric_limits<float>::max();

		// A slab for each dimension.
		int const numberOfDimension = 2;
		for (int i = 0; i < numberOfDimension; i++)
		{
			bool const isRayParallelToSlab = glm::epsilonEqual(rayDirection[i], 0.0f, std::numeric_limits<float>::epsilon());
			if (isRayParallelToSlab)
			{
				bool const isRayOriginInsideSlab = rayOrigin[i] >= aabb.Min[i] && rayOrigin[i] <= aabb.Max[i];
				if (!isRayOriginInsideSlab)
				{
					return false;
				}
				continue;
			}

			float const timeNormalizingMultiplier = 1.0f / rayDirection[i];
			float firstSlabIntersectionTime = (aabb.Min[i] - rayOrigin[i]) * timeNormalizingMultiplier;
			float secondSlabIntersectionTime = (aabb.Max[i] - rayOrigin[i]) * timeNormalizingMultiplier;
			if (firstSlabIntersectionTime > secondSlabIntersectionTime)
			{
				// We want to make sure firstSlabIntersectionTime is always smaller, intersecting with the near plane.
				// firstSlabIntersectionTime is essentially the current slab enter time,
				// secondSlabIntersectionTime is the current slab exit time.
				std::swap(firstSlabIntersectionTime, secondSlabIntersectionTime);
			}

			// Update overall slab enter / exit time interval.
			if (firstSlabIntersectionTime > hitTime)
			{
				hitTime = firstSlabIntersectionTime;
			}

			if (secondSlabIntersectionTime < exitTime)
			{
				exitTime = secondSlabIntersectionTime;
			}

			if (hitTime > exitTime)
			{
				// Exit with no intersection because the slab intersection has become an empty interval.
				return false;
			}
		}

		// The ray intersects with all dimension's slabs.
		hitPoint = rayOrigin + rayDirection * hitTime;
		return true;
	}

	bool RayAABBIntersect2D(glm::vec2 rayOrigin, glm::vec2 rayDirection, float maxDistance, Math::AABB const& aabb, float& hitTime, glm::vec2& hitPoint)
	{
		hitTime = 0.0f; // Could also call it slab 'enter time'. We could set this to -float.max to get hit on the line instead of the ray.
		float exitTime = maxDistance;

		// A slab for each dimension.
		int const numberOfDimension = 2;
		for (int i = 0; i < numberOfDimension; i++)
		{
			bool const isRayParallelToSlab = glm::epsilonEqual(rayDirection[i], 0.0f, std::numeric_limits<float>::epsilon());
			if (isRayParallelToSlab)
			{
				bool const isRayOriginInsideSlab = rayOrigin[i] >= aabb.Min[i] && rayOrigin[i] <= aabb.Max[i];
				if (!isRayOriginInsideSlab)
				{
					return false;
				}
				continue;
			}

			float const timeNormalizingMultiplier = 1.0f / rayDirection[i];
			float firstSlabIntersectionTime = (aabb.Min[i] - rayOrigin[i]) * timeNormalizingMultiplier;
			float secondSlabIntersectionTime = (aabb.Max[i] - rayOrigin[i]) * timeNormalizingMultiplier;
			if (firstSlabIntersectionTime > secondSlabIntersectionTime)
			{
				// We want to make sure firstSlabIntersectionTime is always smaller, intersecting with the near plane.
				// firstSlabIntersectionTime is essentially the current slab enter time,
				// secondSlabIntersectionTime is the current slab exit time.
				std::swap(firstSlabIntersectionTime, secondSlabIntersectionTime);
			}

			// Update overall slab enter / exit time interval.
			if (firstSlabIntersectionTime > hitTime)
			{
				hitTime = firstSlabIntersectionTime;
			}

			if (secondSlabIntersectionTime < exitTime)
			{
				exitTime = secondSlabIntersectionTime;
			}

			if (hitTime > exitTime)
			{
				// Exit with no intersection because the slab intersection has become an empty interval.
				return false;
			}
		}

		// The ray intersects with all dimension's slabs.
		hitPoint = rayOrigin + rayDirection * hitTime;
		return true;
	}
}
