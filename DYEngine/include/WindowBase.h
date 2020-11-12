#pragma once

#include <string>
#include <utility>

namespace DYE
{
    struct WindowProperty
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        explicit WindowProperty(std::string title = "DYE Game",
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
            virtual void* GetNativeWindow() const = 0;

            static WindowBase* Create(const WindowProperty& windowProperty = WindowProperty());
    };
}