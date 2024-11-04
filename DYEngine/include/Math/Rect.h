#pragma once

#include <cstdint>

namespace DYE::Math
{
    struct Rect
    {
        // The value is the same as xMin.
        float X = 0;

        // The value is the same as yMin.
        float Y = 0;

        float Width = 0;
        float Height = 0;

        Rect() = default;

        Rect(float x, float y, float width, float height) : X(x), Y(y), Width(width), Height(height)
        {

        }
    };
}
