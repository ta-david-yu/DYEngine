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
    static void APIENTRY openglCallbackFunction(
            GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam
    ){
        (void)source; (void)type; (void)id;
        (void)severity; (void)length; (void)userParam;

        DYE_LOG("%s", message);

        if (severity==GL_DEBUG_SEVERITY_MEDIUM) {
            DYE_LOG("ABORTING....");
            DYE_ASSERT(false);
        }
    }
#endif

	std::unique_ptr<RenderCommand> RenderCommand::s_Instance {};

	RenderCommand& RenderCommand::GetInstance()
	{
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
    }

    void RenderCommand::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::Clear()
    {
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
			glm::mat4 viewMatrix = renderParameters.Camera.GetViewMatrix();
			auto viewMatrixLoc = glGetUniformLocation(shader.GetID(), DefaultUniformNames::ViewMatrix);
			glCall(glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix)));

			// Camera space to clip space
			float const aspectRatio = renderParameters.Camera.AspectRatio;
			glm::mat4 projectionMatrix = renderParameters.Camera.GetProjectionMatrix(aspectRatio);
			auto projectionMatrixLoc = glGetUniformLocation(shader.GetID(), DefaultUniformNames::ProjectionMatrix);
			glCall(glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));
		}

		// Bind property uniforms
		renderParameters.Material->updateUniformValuesToGPU();

		// Bind mesh (VAO)
		vertexArray.Bind();
		std::uint32_t const indexCount = vertexArray.GetIndexBuffer()->GetCount();
		glCall(glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr));
	}
}