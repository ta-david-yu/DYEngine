#include "Graphics/OpenGL.h"

namespace DYE
{
    std::string g_glErrorMessage;

    std::string glGetErrorMessage()
    {
        return g_glErrorMessage;
    }

    bool glCheckError(const char *functionName)
    {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::string message;
            message += std::string(functionName) + ":   ";

            switch (error)
            {
                case GL_INVALID_ENUM:
                    message += "GL_INVALID_ENUM";
                    break;
                case GL_INVALID_VALUE:
                    message += "GL_INVALID_VALUE";
                    break;
                case GL_INVALID_OPERATION:
                    message += "GL_INVALID_OPERATION";
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    message += "GL_INVALID_FRAMEBUFFER_OPERATION";
                    break;
                case GL_OUT_OF_MEMORY:
                    message += "GL_OUT_OF_MEMORY";
                    break;
                default:
                    message += "GL_UNKNOWN";
                    break;
            }
            g_glErrorMessage = message;
            return true;
        }
        else
        {
            g_glErrorMessage = "";
            return false;
        }
    }
}