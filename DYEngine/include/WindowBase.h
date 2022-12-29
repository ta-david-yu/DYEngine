#pragma once

#include <string>
#include <utility>
#include <memory>

namespace DYE
{
	class ContextBase;

    struct WindowProperty
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        /// Whether or not the window can be resized by the user
        bool IsUserResizable;

        explicit WindowProperty(std::string title,
                                uint32_t width = 1600,
                                uint32_t height = 900,
                                bool isUserResizable = false)
                : Title(std::move(title)),
                  Width(width),
                  Height(height),
                  IsUserResizable(isUserResizable) {}
    };

    class WindowBase
    {
    public:
        // ctor and dtor
        virtual ~WindowBase() = default;

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
		virtual ContextBase& GetContext() const = 0;

        /// Get the pointer to the native window based on the platform library being used. ex. SDLWindow for Windows platform
        /// \return a pointer to the library native window object
        virtual void *GetNativeWindowPtr() const = 0;

        ///  Templated: Get the pointer to the native window based on the platform library being used. ex. SDLWindow for Windows platform
        /// \return a typed pointer to the library native window object
        template<typename T>
        T* GetTypedNativeWindowPtr() const
        {
            return static_cast<T*>(GetNativeWindowPtr());
        }


        /// A factory function that creates a window based on the platform with the given property setup
        /// \param windowProperty the settings property for the created window (name, width, height)
        /// \return a unique pointer to the created window
        static std::unique_ptr<WindowBase> Create(const WindowProperty &windowProperty);


    };
}