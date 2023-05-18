#include "SceneViewEntitySelection.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Camera.h"
#include "Graphics/VertexArray.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"

#include <vector>

namespace DYE::DYEditor
{
	struct SceneViewEntitySelectionSubmission
	{
		EntityInstanceID InstanceID;
		std::shared_ptr<VertexArray> GeometryVAO;
		glm::mat4 ObjectToWorldMatrix;
	};

	struct SceneViewEntitySelectionData
	{
		std::shared_ptr<Material> EntityIDMaterial;
		std::vector<SceneViewEntitySelectionSubmission> Submissions;
		bool ReceiveSubmission = true;
	};

	static SceneViewEntitySelectionData s_Data;

	void SceneViewEntitySelection::InitializeEntityIDShader()
	{
		auto entityIDShader = ShaderProgram::CreateFromFile("Shader_EntityID", "assets\\default\\EntityIDShader.shader");
		entityIDShader->Use();

		s_Data.EntityIDMaterial = Material::CreateFromShader("Material_EntityID", entityIDShader);
	}

	void SceneViewEntitySelection::ReceiveEntityGeometrySubmission(bool value)
	{
		s_Data.ReceiveSubmission = value;
	}

	void SceneViewEntitySelection::RegisterEntityGeometry(EntityInstanceID entityInstanceId,
														  const std::shared_ptr<VertexArray> &geometryVAO,
														  glm::mat4 objToWorldMatrix)
	{
		if (!s_Data.ReceiveSubmission)
		{
			return;
		}

		s_Data.Submissions.emplace_back
		(
			SceneViewEntitySelectionSubmission
			{
				.InstanceID = entityInstanceId,
				.GeometryVAO = geometryVAO,
				.ObjectToWorldMatrix = objToWorldMatrix
			}
		);
	}

	void SceneViewEntitySelection::RenderEntityIDFramebufferWithCamera(Framebuffer &framebuffer, Camera camera)
	{
		DYE_ASSERT_LOG_WARN(s_Data.EntityIDMaterial, "EntityID Material has not been initialized, you might have forgot to call SceneViewEntitySelection first.");

		framebuffer.Bind();

		auto targetDimension = camera.Properties.GetTargetDimension();
		camera.Properties.CachedAutomaticAspectRatio = camera.Properties.GetAutomaticAspectRatioOfDimension(targetDimension);
		Math::Rect const viewportDimension = camera.Properties.GetAbsoluteViewportOfDimension(targetDimension);

		RenderCommand::GetInstance().SetViewport(viewportDimension);
		framebuffer.ClearAttachment(0, -1);
		RenderCommand::GetInstance().ClearDepthStencilOnly();

		// Right now we don't really care about the order of the rendering.
		// TODO: Ideally we want to be able to do custom ordering cuz different render pipelines have different sorting criteria.
		// 		i.e. Pipeline2D wants to sort the sprite based on the sprite's layer & order.
		//		Maybe a SortCriteria struct? or function?
		for (int i = 0; i < s_Data.Submissions.size(); ++i)
		{
			auto instanceID = s_Data.Submissions[i].InstanceID;
			auto &geometryVAO = s_Data.Submissions[i].GeometryVAO;
			auto objectToWorldMatrix = s_Data.Submissions[i].ObjectToWorldMatrix;

			MaterialPropertyBlock materialPropertyBlock;
			materialPropertyBlock.SetInt("_EntityID",  instanceID);

			RenderCommand::GetInstance().DrawIndexedNow
			(
				RenderParameters
				{
					.Camera = camera,
					.Material = s_Data.EntityIDMaterial,
					.PropertyBlock = materialPropertyBlock
				},
				*geometryVAO,
				objectToWorldMatrix
			);
		}

		framebuffer.Unbind();
	}

	void SceneViewEntitySelection::ClearEntityGeometries()
	{
		s_Data.Submissions.clear();
	}
}