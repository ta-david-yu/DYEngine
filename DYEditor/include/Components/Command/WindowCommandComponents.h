#pragma once

#include "Graphics/WindowManager.h"
#include "Core/EditorComponent.h"

#include <glm/glm.hpp>

namespace DYE::DYEditor
{
    struct SetWindowPositionComponent
    {
        glm::vec<2, std::int32_t> Position;
    };

    struct SetWindowSizeComponent
    {
        glm::vec<2, std::uint32_t> Size;
    };

    struct SetWindowTitleComponent
    {
        std::string Title;
    };

    struct CreateWindowComponent
    {
        bool SetPosition = false;

        DYE::Int32 Width = 1600;
        DYE::Int32 Height = 900;
        DYE::Vector2 Position = {0, 0};
        std::string Title = "New Window";
    };

    struct CloseWindowComponent
    {
    };
}