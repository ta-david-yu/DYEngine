#include "Math/Math.h"

#include <glm/gtx/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace DYE::Math
{
	float Lerp(float start, float end, float t)
	{
		return std::lerp(start, end, t);
	}

	glm::vec2 Lerp(glm::vec2 start, glm::vec2 end, float t)
	{
		return { Lerp(start.x, end.x, t), Lerp(start.y, end.y, t) };
	}

	glm::vec3 Lerp(glm::vec3 start, glm::vec3 end, float t)
	{
		return { Lerp(start.x, end.x, t), Lerp(start.y, end.y, t), Lerp(start.z, end.z, t) };
	}

	glm::vec4 Lerp(glm::vec4 start, glm::vec4 end, float t)
	{
		return { Lerp(start.x, end.x, t), Lerp(start.y, end.y, t), Lerp(start.z, end.z, t), Lerp(start.w, end.w, t) };
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

	bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}
}