#pragma once

#include <cstdint>

namespace DYE::Math
{
	struct Rect
	{
		// The value is the same as xMin.
		float X;

		// The value is the same as yMin.
		float Y;

		float Width;
		float Height;

		Rect(float x, float y, float width, float height) : X(x), Y(y), Width(width), Height(height)
		{

		}
	};
}
