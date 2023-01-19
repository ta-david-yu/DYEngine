#include "Math/Math.h"

namespace DYE::Math
{
	float Lerp(float start, float end, float t)
	{
		return std::lerp(start, end, t);
	}

	glm::quat FromToRotation(glm::vec3 from, glm::vec3 to)
	{
		/// Implementation from: https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp
		from = glm::normalize(from);
		to = glm::normalize(to);

		float const cosTheta = dot(from, to);
		glm::vec3 rotationAxis;

		if (cosTheta < -1 + 0.001f)
		{
			// special case when vectors in opposite directions :
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			// This implementation favors a rotation around the Up axis,
			// since it's often what you want to do.
			rotationAxis = cross(glm::vec3(0.0f, 0.0f, 1.0f), from);
			if (length2(rotationAxis) < 0.01 ) 	// bad luck, they were parallel, try again!
			{
				rotationAxis = cross(glm::vec3(1.0f, 0.0f, 0.0f), from);
			}

			rotationAxis = normalize(rotationAxis);
			return angleAxis(glm::radians(180.0f), rotationAxis);
		}

		// Implementation from Stan Melax's Game Programming Gems 1 article
		rotationAxis = cross(from, to);

		float const s = glm::sqrt((1 + cosTheta) * 2);
		float const invs = 1 / s;

		return 	{s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs };
	}

	glm::vec3 ClosestPointOnLine(glm::vec3 point, glm::vec3 linePointA, glm::vec3 linePointB)
	{
		//      P
		//     /|
		//    / |
		//   /  |
		//  A --D-- B
		// We want to find D.

		glm::vec3 const ap = point - linePointA;
		glm::vec3 const ab = linePointB - linePointA;

		// Find the value of t to represent point D on lineAB in parametric form.
		float const t = glm::dot(ap, ab) / glm::dot(ab, ab);

		return linePointA + t * ab;
	}

	glm::vec3 ClosestPointOnLineSegment(glm::vec3 point, glm::vec3 linePointA, glm::vec3 linePointB)
	{
		//      P
		//     /|
		//    / |
		//   /  |
		//  A --D-- B
		// We want to find D.

		glm::vec3 const ap = point - linePointA;
		glm::vec3 const ab = linePointB - linePointA;

		// Find the value of t to represent point D on the line segment AB in parametric form.
		float t = glm::dot(ap, ab) / glm::dot(ab, ab);

		// We want to make sure D is on the segment [a, b].
		t = glm::clamp(t, 0.0f, 1.0f);

		return linePointA + t * ab;
	}
}