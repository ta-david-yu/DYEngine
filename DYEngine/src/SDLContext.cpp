#include "SDLContext.h"

#include <SDL.h>
#include "SDLWindow.h"

namespace DYE
{
    SDLContext::SDLContext(SDLWindow *pSdlWindow) : m_pWindow(pSdlWindow)
    {
        // TODO: add code to check if pSdlWindow is valid
    }

    SDLContext::~SDLContext()
    {
        SDL_GL_DeleteContext(m_pNativeContext);
    }

    void SDLContext::Init()
    {
        m_pNativeContext = SDL_GL_CreateContext(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>());

        // bind the context as the current rendering state to the window
        int status = SDL_GL_MakeCurrent(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>(), m_pNativeContext);
        if (status < 0)
        {
            // MakeCurrent failed
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
        }
    }

    void SDLContext::SwapBuffers()
    {
        SDL_GL_SwapWindow(m_pWindow->GetTypedNativeWindowPtr<SDL_Window>());
    }
}