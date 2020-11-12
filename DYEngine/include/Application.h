#pragma once

#include "WindowBase.h"

#include <memory>

namespace DYE
{
    class Application
    {
        public:
            Application();
            void Run();
        private:
            std::unique_ptr<WindowBase> m_Window;
    };
}
