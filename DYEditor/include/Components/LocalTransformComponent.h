#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

namespace DYE::DYEditor
{
	struct LocalTransformComponent
	{
		glm::vec3 Position {0, 0, 0};
		glm::vec3 Scale {1, 1, 1};
		glm::quat Rotation {glm::vec3 {0, 0, 0}};

		glm::mat4 GetTransformMatrix() const
		{
			glm::mat4 modelMatrix = glm::mat4 {1.0f};
			modelMatrix = glm::translate(modelMatrix, Position);
			modelMatrix = modelMatrix * glm::toMat4(Rotation);
			modelMatrix = glm::scale(modelMatrix, Scale);

			return modelMatrix;
		}

		glm::vec3 GetRight() const { return Rotation * glm::vec3(1, 0, 0); }
		glm::vec3 GetForward() const { return Rotation * glm::vec3(0, 1, 0); }
	};

	namespace Internal
	{
		/// This component is only meant for Editor,
		/// to show rotation as euler angles in degree in the inspector.
		struct TransformEulerAnglesHintComponent
		{
			glm::vec3 Value {0, 0, 0};
		};
	}
}