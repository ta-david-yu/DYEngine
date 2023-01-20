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

	bool RayAABBIntersect2D(glm::vec2 rayOrigin, glm::vec2 rayDirection, Math::AABB const& aabb, DynamicTestResult2D& testResult)
	{
		testResult.HitTime = 0.0f;	// Could also call it slab 'enter time'. We could set this to -float.max to get hit on the line instead of the ray.
		testResult.HitNormal = -glm::normalize(rayDirection); // Set normal to the opposite of ray direction by default.
		float exitTime = std::numeric_limits<float>::max();

		// A slab for each dimension.
		int const numberOfDimension = 2;
		int hitSlabDimensionIndex = -1;	// The dimension index of the slab that ray enters. We use this to calculate hit normal later.
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
			if (firstSlabIntersectionTime > testResult.HitTime)
			{
				testResult.HitTime = firstSlabIntersectionTime;
				hitSlabDimensionIndex = i;
			}

			if (secondSlabIntersectionTime < exitTime)
			{
				exitTime = secondSlabIntersectionTime;
			}

			if (testResult.HitTime > exitTime)
			{
				// Exit with no intersection because the slab intersection has become an empty interval.
				return false;
			}
		}

		if (hitSlabDimensionIndex != -1)
		{
			// Compute hit normal.
			for (int i = 0; i < numberOfDimension; i++)
			{
				float const sign = glm::sign(testResult.HitNormal[i]);
				testResult.HitNormal[i] = (i == hitSlabDimensionIndex)? sign * 1.0f : 0.0f;
			}
		}

		// The ray intersects with all dimension's slabs.
		testResult.HitPoint = rayOrigin + rayDirection * testResult.HitTime;
		return true;
	}

	bool RayAABBIntersect2D(glm::vec2 rayOrigin, glm::vec2 rayDirection, float maxDistance, Math::AABB const& aabb, DynamicTestResult2D& testResult)
	{
		testResult.HitTime = 0.0f; // Could also call it slab 'enter time'. We could set this to -float.max to get hit on the line instead of the ray.
		testResult.HitNormal = -glm::normalize(rayDirection); // Set normal to the opposite of ray direction by default.
		float exitTime = maxDistance;

		// A slab for each dimension.
		int const numberOfDimension = 2;
		int hitSlabDimensionIndex = -1;	// The dimension index of the slab that ray enters. We use this to calculate hit normal later.
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
			if (firstSlabIntersectionTime > testResult.HitTime)
			{
				testResult.HitTime = firstSlabIntersectionTime;
				hitSlabDimensionIndex = i;
			}

			if (secondSlabIntersectionTime < exitTime)
			{
				exitTime = secondSlabIntersectionTime;
			}

			if (testResult.HitTime > exitTime)
			{
				// Exit with no intersection because the slab intersection has become an empty interval.
				return false;
			}
		}

		if (hitSlabDimensionIndex != -1)
		{
			// Compute hit normal.
			for (int i = 0; i < numberOfDimension; i++)
			{
				float const sign = glm::sign(testResult.HitNormal[i]);
				testResult.HitNormal[i] = (i == hitSlabDimensionIndex)? sign * 1.0f : 0.0f;
			}
		}

		// The ray intersects with all dimension's slabs.
		testResult.HitPoint = rayOrigin + rayDirection * testResult.HitTime;
		return true;
	}

	bool RayCircleIntersect(glm::vec2 rayOrigin, glm::vec2 rayDirection, glm::vec2 center, float radius, DynamicTestResult2D& testResult)
	{
		testResult.HitNormal = -glm::normalize(rayDirection); // Set normal to the opposite of ray direction by default.

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

		// A negative discriminant corresponds to ray missing circle
		if (discriminant < 0.0f)
		{
			return false;
		}

		// Ray now found to intersect circle, compute smallest t value of intersection
		testResult.HitTime = (-b - glm::sqrt(discriminant)) / a;

		// If t is negative, ray started inside circle so clamp t to zero
		bool const insideCircle = testResult.HitTime < 0.0f;
		if (insideCircle)
		{
			testResult.HitTime = 0.0f;
		}

		testResult.HitPoint = rayOrigin + testResult.HitTime * rayDirection;
		if (!insideCircle)
		{
			// If rayOrigin is not inside, the hit normal would be 'circle center' to 'hit point'.
			testResult.HitNormal = testResult.HitPoint - center;
		}

		return true;
	}

	bool MovingCircleAABBIntersect(glm::vec2 center, float radius, glm::vec2 direction, const AABB &aabb, DynamicTestResult2D& testResult)
	{
		// Compute the AABB resulting from expanding aabb by circle's radius.
		AABB expandedAABB = aabb;
		expandedAABB.Min.x -= radius; expandedAABB.Min.y -= radius;
		expandedAABB.Max.x += radius; expandedAABB.Max.y += radius;

		// Do a ray & expanded AABB intersect test.
		if (!RayAABBIntersect2D(center, direction, expandedAABB, testResult) || testResult.HitTime > 1.0f)
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
		bool const inSlabX = testResult.HitPoint.x >= aabb.Min.x && testResult.HitPoint.x <= aabb.Max.x;
		bool const inSlabY = testResult.HitPoint.y >= aabb.Min.y && testResult.HitPoint.y <= aabb.Max.y;

		bool const isEdgeRegion = inSlabX || inSlabY;
		if (isEdgeRegion)
		{
			return true;
		}

		// The hitPoint is inside vertex region, do a ray & circle intersect.
		glm::vec2 corner;
		corner.x = (testResult.HitPoint.x < aabb.Min.x)? aabb.Min.x : aabb.Max.x;
		corner.y = (testResult.HitPoint.y < aabb.Min.y)? aabb.Min.y : aabb.Max.y;

		return RayCircleIntersect(center, direction, corner, radius, testResult);
	}
}
