#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/orthonormalize.hpp>

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

	struct LocalToWorldComponent
	{
		glm::mat4 Matrix = glm::mat4 {1.0f};

		glm::vec3 GetRight() const { return {Matrix[0][0], Matrix[0][1], Matrix[0][2]}; }
		glm::vec3 GetUp() const { return {Matrix[1][0], Matrix[1][1], Matrix[1][2]}; }
		glm::vec3 GetForward() const { return {Matrix[2][0], Matrix[2][1], Matrix[2][2]}; }

		glm::vec3 GetPosition() const { return {Matrix[3][0], Matrix[3][1], Matrix[3][2]}; }
		glm::quat GetRotation() const
		{
			glm::mat3 rotationMatrix = glm::orthonormalize(glm::mat3 {Matrix});
			return { rotationMatrix };
		}
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