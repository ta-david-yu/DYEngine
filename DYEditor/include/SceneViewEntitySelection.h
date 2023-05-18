#pragma once

#include "Core/Entity.h"

#include <glm/glm.hpp>
#include <memory>

namespace DYE
{
	class VertexArray;
	class Framebuffer;
	class Camera;
}

namespace DYE::DYEditor
{
	struct SceneViewEntitySelection
	{
		static void ReceiveEntityGeometrySubmission(bool value);
		static void InitializeEntityIDShader();
		static void RegisterEntityGeometry(EntityInstanceID entityInstanceId,
										   const std::shared_ptr<VertexArray> &geometryVAO,
										   glm::mat4 objToWorldMatrix);
		static void RenderEntityIDFramebufferWithCamera(Framebuffer &framebuffer, Camera camera);
		static void ClearEntityGeometries();
	};
}