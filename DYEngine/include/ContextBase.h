#pragma once

namespace DYE
{
    class WindowBase;

    /// A Context stores the state associated with the renderer API, such as OpenGL. It always belongs to a WindowBase (life-time shorter than WindowBase)
    class ContextBase
    {
    public:
        virtual ~ContextBase() = default;

        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;

        /// A factory function that creates a graphics context based on the platform
        /// \param pWindow a pointer to the window object that the created context belongs to (i.e. SDLWindow)
        /// \return a unique pointer to the created window
        static std::unique_ptr<ContextBase> Create(WindowBase* pWindow);
    };
}