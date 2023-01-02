#pragma once

#include "ContextBase.h"

namespace DYE
{
    class SDLWindow;

    /// A GraphicsContext that stores the states associated with SDL2 OpenGL
    class SDLContext : public ContextBase
    {
    public:
        ~SDLContext() override;

		void * GetNativeContextPtr() const override;
		void MakeCurrentForWindow(WindowBase const& window) override;

	protected:
		void init(WindowBase* pWindow) override;

    private:
        /// The pointer to the native Context object (SDL)
        void* m_pNativeContext;
    };
}