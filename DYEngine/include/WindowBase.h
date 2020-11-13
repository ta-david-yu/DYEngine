#pragma once

#include <string>
#include <utility>
#include <memory>

namespace DYE
{
    struct WindowProperty
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        explicit WindowProperty(std::string title,
                       uint32_t width = 1600,
                       uint32_t height = 900)
                       : Title(std::move(title)),
                       Width(width),
                       Height(height) { }
    };

    class WindowBase
    {
        public:
            // ctor and dtor
            virtual ~WindowBase() = default;

            virtual void OnUpdate() = 0;

            /// Get the pointer to the native window based on the platform library being used. ex. SDLWindow for Windows platform
            /// \return a pointer to the library native window object
            virtual void* GetNativeWindow() const = 0;

            /// Create a window based on the property setup
            /// \param windowProperty the settings property for the created window (name, width, height)
            /// \return a unique pointer to the created window
            static std::unique_ptr<WindowBase> Create(const WindowProperty& windowProperty);
    };
}