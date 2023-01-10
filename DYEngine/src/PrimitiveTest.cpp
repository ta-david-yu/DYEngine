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

	bool RayCircleIntersect(glm::vec2 rayOrigin, glm::vec2 rayDirection, glm::vec2 center, float radius, float &hitTime, glm::vec2 &hitPoint)
	{
		glm::vec2 const cp = rayOrigin - center;
		float const a = glm::dot(rayDirection, rayDirection);
		float const b = glm::dot(cp, rayDirection);
		float const c = glm::dot(cp, cp) - radius * radius;

		bool const rayOriginOutsideCircle = c > 0.0f;
		bool const rayPointingAwayFromCircle = b > 0.0f;
		if (rayOriginOutsideCircle && rayPointingAwayFromCircle)
		{
			return false;
		}

		// Solve quadratic equation (ray & circle equation) using t = -b +/- sqrt(b^2 - 4ac).
		// Reference: Real-Time Collision Detection by Christer Ericson.
		float const discriminant = b * b - a * c;

		// A negative discriminant corresponds to ray missing sphere
		if (discriminant < 0.0f)
		{
			return false;
		}

		// Ray now found to intersect sphere, compute smallest t value of intersection
		hitTime = (-b - glm::sqrt(discriminant)) / a;

		// If t is negative, ray started inside sphere so clamp t to zero
		if (hitTime < 0.0f)
		{
			hitTime = 0.0f;
		}
		hitPoint = rayOrigin + hitTime * rayDirection;
		return true;
	}

	bool MovingCircleAABBIntersect(glm::vec2 center, float radius, glm::vec2 direction, const AABB &aabb, float &hitTime)
	{
		// Compute the AABB resulting from expanding aabb by circle's radius.
		AABB expandedAABB = aabb;
		expandedAABB.Min.x -= radius; expandedAABB.Min.y -= radius;
		expandedAABB.Max.x += radius; expandedAABB.Max.y += radius;

		// Do a ray & expanded AABB intersect test.
		glm::vec2 hitPoint;
		if (!RayAABBIntersect2D(center, direction, expandedAABB, hitTime, hitPoint) || hitTime > 1.0f)
		{
			// The ray doesn't intersect with the expanded AABB
			// OR it intersects but at a point where it's farther than direction vector.
			return false;
		}

		// Check which Voronoi region hitPoint lies in: edge region OR corner region
		//               max
		//    c_|__e__|_c
		//    e |     | e
		//    __|_____|__   slabY
		//    c |  e  | c
		// min
		//       slabX
		bool const inSlabX = hitPoint.x >= aabb.Min.x && hitPoint.x <= aabb.Max.x;
		bool const inSlabY = hitPoint.y >= aabb.Min.y && hitPoint.y <= aabb.Max.y;

		bool const isEdgeRegion = inSlabX || inSlabY;
		if (isEdgeRegion)
		{
			return true;
		}

		// The hitPoint is inside vertex region, do a ray & circle intersect.
		glm::vec2 corner;
		corner.x = (hitPoint.x < aabb.Min.x)? aabb.Min.x : aabb.Max.x;
		corner.y = (hitPoint.y < aabb.Min.y)? aabb.Min.y : aabb.Max.y;

		return RayCircleIntersect(center, direction, corner, radius, hitTime, hitPoint);
	}
}
