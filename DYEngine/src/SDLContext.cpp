#include "Graphics/SDLContext.h"

#include "Graphics/SDLWindow.h"
#include "Util/Logger.h"

#include <SDL.h>
#include <glad/glad.h>

namespace DYE
{
    SDLContext::~SDLContext()
    {
        SDL_GL_DeleteContext(m_pNativeContext);
    }

    void SDLContext::init(WindowBase *pWindow)
    {
        m_pNativeContext = SDL_GL_CreateContext(pWindow->GetTypedNativeWindowPtr<SDL_Window>());
    }

    void *SDLContext::GetNativeContextPtr() const
    {
        return m_pNativeContext;
    }

    void SDLContext::MakeCurrentForWindow(WindowBase const &window)
    {
        // bind the context as the current rendering state to the window
        int const status = SDL_GL_MakeCurrent(window.GetTypedNativeWindowPtr<SDL_Window>(), GetNativeContextPtr());
        if (status < 0)
        {
            // MakeCurrent failed
            DYE_LOG("SDLContext: SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
        }

        // Load OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
        {
            DYE_LOG_ERROR("SDLContext: Couldn't initialize glad");
        }
    }
}