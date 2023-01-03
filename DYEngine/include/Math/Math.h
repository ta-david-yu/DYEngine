#pragma once

#include <cstdint>
#include <cmath>

namespace DYE::Math
{
	float Lerp(float start, float end, float t)
	{
		return std::lerp(start, end, t);
	}
}