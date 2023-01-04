#include "Graphics/ContextBase.h"

#include "Graphics/SDLWindow.h"
#include "Graphics/SDLContext.h"
#include "Util/Logger.h"

#include <SDL.h>

namespace DYE
{
    std::shared_ptr<ContextBase> ContextBase::Create(WindowBase *pWindow)
    {
		std::shared_ptr<ContextBase> context = std::make_shared<SDLContext>();
		// TODO: add other context types. For instance, GLFWContext
		// 	...

		context->init(pWindow);
        return std::move(context);
    }

	bool ContextBase::SetVSyncCountForCurrentContext(int count)
	{
		// TODO: add other platform types calls. For instance, GLFWContext call
		// 	...

		bool const success = SDL_GL_SetSwapInterval(count) == 0;
		if (!success)
		{
			DYE_LOG_ERROR("SetVSyncCount(%d) failed: %s", count, SDL_GetError());
		}

		return success;
	}
}