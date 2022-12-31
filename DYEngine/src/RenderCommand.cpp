#include "Logger.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/OpenGL.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"

#include <SDL.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

namespace DYE
{
#ifdef DYE_OPENGL_DEBUG
    static void APIENTRY openglCallbackFunction
		(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam
    	)
	{
        (void)source; (void)type; (void)id;
        (void)severity; (void)length; (void)userParam;

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:
				DYE_LOG_ERROR("[OpenGL Debug HIGH] %s", message);
				DYE_ASSERT(false);
			case GL_DEBUG_SEVERITY_MEDIUM:
				DYE_LOG_WARN("[OpenGL Debug HIGH] %s", message);
		}
    }
#endif

	std::unique_ptr<RenderCommand> RenderCommand::s_Instance {};

	RenderCommand& RenderCommand::GetInstance()
	{
		DYE_ASSERT_RELEASE(
			s_Instance != nullptr &&
			"You might have forgot to call RenderCommand::InitSingleton beforehand?");
		return *s_Instance;
	}

    void RenderCommand::InitSingleton()
    {
#ifdef DYE_OPENGL_DEBUG
        // Enable the debug callback
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true
        );
#endif
		s_Instance = std::make_unique<RenderCommand>();
    }

    void RenderCommand::SetViewport(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height)
    {
        glViewport(x, y, width, height);
		glScissor(x, y, width, height);
    }

	void RenderCommand::SetViewport(Math::Rect viewportRect)
	{
		glViewport(viewportRect.X, viewportRect.Y, viewportRect.Width, viewportRect.Height);
		glScissor(viewportRect.X, viewportRect.Y, viewportRect.Width, viewportRect.Height);
	}

    void RenderCommand::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::Clear()
    {
		// Enable depth mask first so glClear() call actually clears depth buffer.
		glCall(glDepthMask(GL_TRUE));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

	void RenderCommand::DrawIndexedNow(VertexArray const& vertexArray)
	{
		// Bind mesh (VAO)
		vertexArray.Bind();
		std::uint32_t const indexCount = vertexArray.GetIndexBuffer()->GetCount();
		glCall(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr));
	}

	void RenderCommand::DrawIndexedNow(RenderParameters const& renderParameters, VertexArray const& vertexArray, glm::mat4 objectToWorldMatrix)
	{
		auto& shader = renderParameters.Material->GetShaderProgram();
		auto& camera = renderParameters.Camera;

		// Set render state
		shader.GetDefaultRenderState().Apply();

		// Bind shader
		shader.Use();

		// Bind built-in uniforms (matrices)
		{
			// Local to world space
			auto modelMatrixLoc = glGetUniformLocation(shader.GetID(), DefaultUniformNames::ModelMatrix);
			glCall(glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, glm::value_ptr(objectToWorldMatrix)));

			// World space to camera space
			glm::mat4 viewMatrix = camera.GetViewMatrix();
			auto viewMatrixLoc = glGetUniformLocation(shader.GetID(), DefaultUniformNames::ViewMatrix);
			glCall(glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix)));

			// Camera space to clip space
			float const aspectRatio = camera.GetAspectRatio();
			glm::mat4 projectionMatrix = camera.GetProjectionMatrix(aspectRatio);
			auto projectionMatrixLoc = glGetUniformLocation(shader.GetID(), DefaultUniformNames::ProjectionMatrix);
			glCall(glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));
		}

		// Bind property uniforms on the material
		renderParameters.Material->updateUniformValuesToGPU();

		// Bind property values on the property block
		renderParameters.PropertyBlock.updatePropertyValuesToGPU(shader);

		// Bind mesh (VAO)
		vertexArray.Bind();
		std::uint32_t const indexCount = vertexArray.GetIndexBuffer()->GetCount();
		glCall(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr));
	}
}