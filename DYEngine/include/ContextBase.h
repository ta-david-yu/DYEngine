#pragma once

#include <memory>

namespace DYE
{
    class WindowBase;

    /// A Context stores the state associated with the renderer API, such as OpenGL. It always belongs to a WindowBase (life-time shorter than WindowBase)
    class ContextBase
    {
    public:
		/// A factory function that creates a graphics context based on the platform
		/// \param pWindow a pointer to the window object that the created context belongs to (i.e. SDLWindow)
		/// \return a unique pointer to the created window
		static std::shared_ptr<ContextBase> Create(WindowBase* pWindow);

		/// Set vSync count for the current context.
		/// Therefore you might want to call MakeCurrentForWindow function first before setting the vsync.
		/// \param count
		/// \return true if the call succeeds. Otherwise false.
		static bool SetVSyncCountForCurrentContext(int count);

		ContextBase() = default;
        virtual ~ContextBase() = default;

		/// Get the pointer to the native context based on the platform library being used. ex. SDLContext for Windows platform
		/// \return a pointer to the library native context object
		virtual void *GetNativeContextPtr() const = 0;

		/// Make the context as the current context for rendering on the given window.
		virtual void MakeCurrentForWindow(WindowBase const* pWindow) = 0;

	protected:
		/// Since we need to make the default ctor trivial (to avoid throwing exceptions),
		/// init() is used to initialize the internal context state instead.
		virtual void init(WindowBase* pWindow) = 0;

    };
}