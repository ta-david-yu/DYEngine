#pragma once

#include "WindowBase.h"

#include <memory>

namespace DYE
{
    class Application
    {
        public:
            /// Application Constructor
            /// \param windowName: The name shown at the top of the app window
            explicit Application(const std::string& windowName = "DYE App");

            /// App Entrypoint
            void Run();
        private:
            std::unique_ptr<WindowBase> m_Window;
    };
}
