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
		testResult.HitCentroid = rayOrigin + rayDirection * testResult.HitTime;
		testResult.HitPoint = testResult.HitCentroid;
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
		testResult.HitCentroid = rayOrigin + rayDirection * testResult.HitTime;
		testResult.HitPoint = testResult.HitCentroid;
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

		testResult.HitCentroid = rayOrigin + testResult.HitTime * rayDirection;
		testResult.HitPoint = testResult.HitCentroid;
		if (!insideCircle)
		{
			// If rayOrigin is not inside, the hit normal would be 'circle center' to 'hit point'.
			testResult.HitNormal = testResult.HitCentroid - center;
		}

		return true;
	}

	bool MovingCircleAABBIntersect(glm::vec2 center, float radius, glm::vec2 direction, const AABB &aabb, DynamicTestResult2D& testResult)
	{
		testResult.HitNormal = -glm::normalize(direction); // Set normal to the opposite of moving direction by default.

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
		bool const inSlabX = testResult.HitCentroid.x >= aabb.Min.x && testResult.HitCentroid.x <= aabb.Max.x;
		bool const inSlabY = testResult.HitCentroid.y >= aabb.Min.y && testResult.HitCentroid.y <= aabb.Max.y;

		bool const isInsideAABB = inSlabX && inSlabY;
		if (isInsideAABB)
		{
			return true;
		}

		bool const isEdgeRegion = inSlabX || inSlabY;
		if (inSlabX)
		{
			testResult.HitNormal.x = 0.0f;
		}
		if (inSlabY)
		{
			testResult.HitNormal.y = 0.0f;
		}

		if (isEdgeRegion)
		{
			testResult.HitPoint = testResult.HitCentroid - glm::normalize(testResult.HitNormal) * radius;
			return true;
		}

		// The hitPoint is inside vertex region, do a ray & circle intersect.
		glm::vec2 corner;
		corner.x = (testResult.HitCentroid.x < aabb.Min.x) ? aabb.Min.x : aabb.Max.x;
		corner.y = (testResult.HitCentroid.y < aabb.Min.y) ? aabb.Min.y : aabb.Max.y;

		bool const rayExpandedCircleIntersect = RayCircleIntersect(center, direction, corner, radius, testResult);
		testResult.HitPoint = testResult.HitCentroid - testResult.HitNormal;
		return rayExpandedCircleIntersect;
	}

	bool MovingAABBAABBIntersect2D(AABB const& aabb, glm::vec2 direction, AABB const& otherAABB, DynamicTestResult2D& testResult)
	{
		if (AABBAABBIntersect2D(aabb, otherAABB))
		{
			// Early return if both AABBs initially overlapping.
			testResult.HitCentroid = aabb.Center();
			testResult.HitPoint = testResult.HitCentroid;
			testResult.HitTime = 0;
			return true;
		}

		// We approach this problem by calculating the first/last contact time on each axis.
		// All axes first/last should overlap with each other.
		constexpr int const numberOfAxes = 2;
		float firstContactTime = 0.0f;
		float lastContactTime = 1.0f;
		for (int i = 0; i < numberOfAxes; ++i)
		{
			if (direction[i] > 0.0f)
			{
				if (aabb.Min[i] > otherAABB.Max[i]) return false;	// AABB is moving away from OtherAABB on axis i.
				if (aabb.Max[i] < otherAABB.Min[i]) firstContactTime = glm::max((otherAABB.Min[i] - aabb.Max[i]) / direction[i], firstContactTime);
				if (aabb.Min[i] < otherAABB.Max[i]) lastContactTime = glm::min((otherAABB.Max[i] - aabb.Min[i]) / direction[i], lastContactTime);
			}
			if (direction[i] < 0.0f)
			{
				if (aabb.Max[i] < otherAABB.Min[i]) return false;	// AABB is moving away from OtherAABB on axis i.
				// Notice that because direction[i] is negative, the distance diff calculation is smaller value - bigger value.
				if (aabb.Min[i] > otherAABB.Max[i]) firstContactTime = glm::max((otherAABB.Max[i] - aabb.Min[i]) / direction[i], firstContactTime);
				if (aabb.Max[i] > otherAABB.Min[i]) lastContactTime = glm::min((otherAABB.Min[i] - aabb.Max[i]) / direction[i], lastContactTime);
			}
		}

		if (firstContactTime > lastContactTime)
		{
			// AABBs never overlap on all axes at the same time.
			return false;
		}

		testResult.HitCentroid = aabb.Center() + firstContactTime * glm::vec3 {direction, 0};
		testResult.HitPoint = testResult.HitCentroid;
		testResult.HitTime = firstContactTime;
		return true;

		// TODO: Implement actual AABB x AABB test
		auto center = aabb.Center();
		return MovingCircleAABBIntersect(center, (aabb.Max.x - aabb.Min.x) * 0.5f, direction, otherAABB, testResult);
	}
}
