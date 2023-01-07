#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cstdint>
#include <cmath>

namespace DYE::Math
{
	float Lerp(float start, float end, float t);
	glm::quat FromToRotation(glm::vec3 from, glm::vec3 to);
}