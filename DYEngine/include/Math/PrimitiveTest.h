#pragma once

#include "Math/AABB.h"

namespace DYE::Math
{
    struct DynamicTestResult2D
    {
        float HitTime {0.0f};

        // This will be the same as HitPoint if the moving object is a ray.
        // Otherwise, it will be the position of the moving object when the hit happens
        // (i.e. the center location of circle when circle hits the object tested against).
        glm::vec2 HitCentroid {0.0f, 0.0f};

        glm::vec2 HitPoint {0.0f, 0.0f};

        // Note that the length is not guaranteed to be 1.
        // If a hit occurs starting inside the test target, the collision normal is the opposite direction of the movement direction.
        glm::vec2 HitNormal {0.0f, 0.0f};
    };

    bool CircleCircleIntersect(glm::vec2 aCenter, float aRadius, glm::vec2 bCenter, float bRadius);
    bool IsCircleInsideCircle(glm::vec2 aCenter, float aRadius, glm::vec2 bCenter, float bRadius);

    bool AABBAABBIntersect2D(AABB const &a, AABB const &b);
    bool AABBAABBIntersect(AABB const &a, AABB const &b);

    bool AABBSphereIntersect(AABB const &a, glm::vec3 center, float radius);
    bool AABBCircleIntersect(AABB const &a, glm::vec2 center, float radius);

    /// \param hitTime only valid if the function returns true. The normalized hit time based on rayDirection length,
    /// therefore if the value is between [0, 1], it means the hit point lies between points (origin) & (origin + direction).
    /// \param hitPoint only valid if the function returns true.
    bool RayAABBIntersect2D(glm::vec2 rayOrigin, glm::vec2 rayDirection, Math::AABB const &aabb, DynamicTestResult2D &testResult);

    /// \param hitTime only valid if the function returns true. The normalized hit time based on rayDirection length.
    /// \param hitPoint only valid if the function returns true.
    bool RayAABBIntersect2D(glm::vec2 rayOrigin, glm::vec2 rayDirection, float maxDistance, Math::AABB const &aabb, DynamicTestResult2D &testResult);

    bool RayCircleIntersect(glm::vec2 rayOrigin, glm::vec2 rayDirection, glm::vec2 center, float radius, DynamicTestResult2D &testResult);

    bool MovingCircleAABBIntersect(glm::vec2 center, float radius, glm::vec2 direction, AABB const &aabb, DynamicTestResult2D &testResult);

    bool MovingAABBAABBIntersect2D(AABB const &aabb, glm::vec2 direction, AABB const &otherAABB, DynamicTestResult2D &testResult);
}
