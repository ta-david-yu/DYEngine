#pragma once

#include <glm/glm.hpp>

namespace DYE
{
	class Color
	{
	public:
		constexpr static const glm::vec4 Red { 1, 0, 0, 1 };
		constexpr static const glm::vec4 Green = {0, 1, 0, 1};
		constexpr static const glm::vec4 Blue = {0, 0, 1, 1};
		constexpr static const glm::vec4 Yellow = {1, 0.92f, 0.016f, 1};

		constexpr static const glm::vec4 Black = {0, 0, 0, 1};
		constexpr static const glm::vec4 White = {1, 1, 1, 1};
		constexpr static const glm::vec4 Gray = {0.5f, 0.5f, 0.5f, 1};
		constexpr static const glm::vec4 Clear = {0, 0, 0, 0};
	};
}