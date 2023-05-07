#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <cmath>

namespace DYE::Math
{
	float Lerp(float start, float end, float t);
	glm::vec2 Lerp(glm::vec2 start, glm::vec2 end, float t);
	glm::vec3 Lerp(glm::vec3 start, glm::vec3 end, float t);
	glm::vec4 Lerp(glm::vec4 start, glm::vec4 end, float t);
	glm::quat FromToRotation(glm::vec3 from, glm::vec3 to);
	glm::vec3 ClosestPointOnLine(glm::vec3 point, glm::vec3 linePointA, glm::vec3 linePointB);
	glm::vec3 ClosestPointOnLineSegment(glm::vec3 point, glm::vec3 linePointA, glm::vec3 linePointB);
	bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &outTranslation, glm::vec3 &outRotation, glm::vec3 &outScale);
}