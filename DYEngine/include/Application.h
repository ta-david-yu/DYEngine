#pragma once

#include "WindowBase.h"
#include "Time.h"

#include <memory>

namespace DYE
{
    class Application
    {
    public:
        Application() = delete;

        Application(const Application &) = delete;

        /// Application Constructor
        /// \param windowName: The name shown at the top of the app window
        explicit Application(const std::string &windowName = "DYE App", int framePerSecond = 60);

        /// App Entrypoint
        void Run();

    private:

        /// The main rendering window
        std::unique_ptr<WindowBase> m_Window;

        /// Time for counting delta time per frame
        Time m_Time;

        bool m_IsRunning = false;
    };
}
