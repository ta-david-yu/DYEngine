#pragma once

#include <cstdint>

namespace DYE::Math
{
	struct Rect
	{
		float X;
		float Y;
		float Width;
		float Height;

		Rect(float x, float y, float width, float height) : X(x), Y(y), Width(width), Height(height)
		{

		}
	};
}
