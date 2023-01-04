#pragma once

#include "glm/glm.hpp"

#include <string>
#include <utility>
#include <memory>

namespace DYE
{
	class ContextBase;

	using WindowID = std::uint32_t;

	enum class FullScreenMode
	{
		Window
		, FullScreen						/// Fullscreen and force changing the resolution of the display.
		, FullScreenWithDesktopResolution 	/// Fullscreen but doesn't force the resolution of the display to the application resolution
	};

    struct WindowProperty
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

		FullScreenMode FullScreeMode = FullScreenMode::Window;

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
		/// A factory function that creates a window based on the platform with the given property setup.
		/// Normally you should avoid calling this directly, it's only for engine internal use. Call WindowManager::CreateWindow instead.
		/// \param windowProperty the settings property for the created window (name, width, height)
		/// \return a unique pointer to the created window
		static std::unique_ptr<WindowBase> Create(const WindowProperty &windowProperty);

		// ctor and dtor
        virtual ~WindowBase() = default;

        virtual void OnUpdate() = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
		virtual glm::vec<2, std::int32_t> GetPosition() const = 0;
		virtual FullScreenMode GetFullScreenMode() const = 0;
		virtual int GetDisplayIndex() const = 0;
		std::shared_ptr<ContextBase> GetContext() const;

        /// Get the pointer to the native window based on the platform library being used. ex. SDLWindow for Windows platform
        /// \return a pointer to the library native window object
        virtual void *GetNativeWindowPtr() const = 0;

		/// Get the id of the window. The id implementation depends on the underlying platform library but it should always be unique across different windows.
		/// For instance, a SDLWindow uses SDL_GetWindowID as the value.
		/// \return the id of the window
		virtual WindowID GetWindowID() const = 0;

        ///  Templated: Get the pointer to the native window based on the platform library being used. ex. SDLWindow for Windows platform
        /// \return a typed pointer to the library native window object
        template<typename T>
        T* GetTypedNativeWindowPtr() const
        {
            return static_cast<T*>(GetNativeWindowPtr());
        }

		void SetContext(std::shared_ptr<ContextBase> context);
		virtual bool SetFullScreenMode(FullScreenMode mode) = 0;
		virtual bool SetBorderedIfWindowed(bool isBordered) = 0;
		virtual void SetWindowSize(std::uint32_t width, std::uint32_t height) = 0;

		/// Set the size of the window and set the position of it as if the window's position is the same if its anchor is the center of the window.
		/// \param width
		/// \param height
		/// \return the new position
		glm::vec<2, std::int32_t> SetWindowSizeUsingWindowCenterAsAnchor(std::uint32_t width, std::uint32_t height);
		virtual void SetWindowPosition(std::int32_t x, std::int32_t y) = 0;

		/// Set the window position to the center of the screen.
		virtual void CenterWindow() = 0;

		void MakeCurrent();

	protected:
		/// The underlying GraphicsContext
		std::shared_ptr<ContextBase> m_Context;
	};
}