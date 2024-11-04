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
        /// 3D Affine Matrix
        glm::mat4 Matrix = glm::mat4 {1.0f};

        inline glm::vec3 GetRight() const { return {Matrix[0][0], Matrix[0][1], Matrix[0][2]}; }
        inline glm::vec3 GetUp() const { return {Matrix[1][0], Matrix[1][1], Matrix[1][2]}; }
        inline glm::vec3 GetForward() const { return {Matrix[2][0], Matrix[2][1], Matrix[2][2]}; }

        inline glm::vec3 GetPosition() const { return {Matrix[3][0], Matrix[3][1], Matrix[3][2]}; }
        inline glm::quat GetRotation() const
        {
            glm::mat3 rotationMatrix = glm::orthonormalize(glm::mat3 {Matrix});
            return {rotationMatrix};
        }

        /// Note that if you have a parent transform with scale and a child that is arbitrarily rotated, the scale will be skewed.
        /// This scale value wouldn't be able to represent the skew properly.
        [[deprecated("You should almost never use this function")]]
        glm::vec3 GetLossyScale() const
        {
            // TODO:
            /*
            Implementation from Unity

            Matrix3x3f Transform::GetWorldScale () const
            {
                Matrix3x3f invRotation;
                QuaternionToMatrix (Inverse (GetRotation ()), invRotation);
                Matrix3x3f scaleAndRotation = GetWorldRotationAndScale ();
                return invRotation * scaleAndRotation;
            }

            Vector3f Transform::GetWorldScaleLossy () const
            {
                Matrix3x3f rot = GetWorldScale ();
                return Vector3f (rot.Get (0, 0), rot.Get (1, 1), rot.Get (2, 2));
            }

            glm::mat3 rotationMatrix = glm::orthonormalize(glm::mat3 {Matrix});
            glm::mat3 inverseRotationMatrix = glm::inverse(rotationMatrix);
            glm::mat3 scaleAndRotationMatrix = glm::mat3 {Matrix};
            glm::mat3 worldScale = inverseRotationMatrix * scaleAndRotationMatrix;
            return glm::vec3 { worldScale[0][0], worldScale[1][1], worldScale[2][2] };

             */

            glm::vec3 scale;
            scale.x = glm::length(Matrix[0]);
            scale.y = glm::length(Matrix[1]);
            scale.z = glm::length(Matrix[2]);
            return scale;
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