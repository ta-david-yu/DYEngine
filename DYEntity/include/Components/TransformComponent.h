#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace DYE::DYEntity
{
	struct TransformComponent
	{
		glm::vec3 Position {0, 0, 0};
		glm::vec3 Scale {1, 1, 1};
		glm::quat Rotation {glm::vec3 {0, 0, 0}};
	};
}