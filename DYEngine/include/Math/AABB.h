#pragma once

#include <glm/glm.hpp>

namespace DYE::Math
{
    /// Represents an axis-aligned bounding box.
    struct AABB
    {
        glm::vec3 Min;
        glm::vec3 Max;

        static AABB CreateFromCenter(glm::vec3 center, glm::vec3 size)
        {
            return AABB(center - size * 0.5f, center + size * 0.5f);
        }

        AABB(glm::vec3 min, glm::vec3 max) : Min(min), Max(max) {}

        glm::vec3 Center() const { return Min + (Max - Min) * 0.5f; }

        /// Check if a 3d point is inside AABB.
        /// \param point
        /// \return true if a point is inside AABB.
        bool Contains(glm::vec3 point) const;

        /// Check if a point is inside AABB.
        /// \param point
        /// \return true if a point is inside AABB.
        bool Contains2D(glm::vec2 point) const;

        /// The closest point in AABB to the given point
        /// \param point
        /// \return the closest point. If the given point is inside AABB, the point itself is returned.
        glm::vec3 ClosestPoint(glm::vec3 point) const;

        /// The closest point in AABB to the given point
        /// \param point
        /// \return the closest point. If the given point is inside AABB, the point itself is returned.
        glm::vec2 ClosestPoint2D(glm::vec2 point) const;

        /// The smallest squared distance between AABB and the given point.
        /// \param point
        /// \return the squared distance between AABB and the given point. 0 if the point is inside AABB.
        float SqrDistance(glm::vec3 point) const;

        /// The smallest squared distance between AABB and the given point.
        /// \param point
        /// \return the squared distance between AABB and the given point. 0 if the point is inside AABB.
        float SqrDistance2D(glm::vec2 point) const;
    };
}