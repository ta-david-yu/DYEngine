#pragma once

#include <glm/glm.hpp>

namespace DYE::MiniGame
{
	struct PongPlayer
	{
		struct Settings
		{
			int ID;
			glm::vec3 InitialPaddleLocation;
			glm::vec3 GoalAreaCenter;
			glm::vec3 GoalAreaSize;
			glm::vec3 Color;
		} Settings;

		struct State
		{
			int Score;
		} State;
	};
}