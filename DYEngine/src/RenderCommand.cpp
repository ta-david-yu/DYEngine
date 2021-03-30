#include "Graphics/RenderCommand.h"
#include "Logger.h"
#include "Graphics/OpenGL.h"

#include <SDL.h>
#include <glad/glad.h>

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

    void RenderCommand::Init()
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

        // Enable GL functionalities
        glCall(glEnable(GL_BLEND));
        glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        // Enable Depth Test by default
        glCall(glEnable(GL_DEPTH_TEST));
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

    void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray, std::uint32_t indexCount)
    {
        uint32_t count = indexCount == 0? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
        glCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
        //glBindTexture(GL_TEXTURE_2D, 0);
    }
}