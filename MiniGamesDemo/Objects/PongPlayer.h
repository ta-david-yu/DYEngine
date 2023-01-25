#pragma once

#include <glm/glm.hpp>

namespace DYE::MiniGame
{
	struct PongPlayer
	{
		struct Settings
		{
			int ID;
			glm::vec3 MainPaddleLocation;
			// The location offset for ball to attach at.
			glm::vec2 MainPaddleAttachOffset;
			glm::vec3 HomebaseCenter;
			glm::vec3 HomebaseSize;
			glm::vec3 Color;
		} Settings;

		struct State
		{
			int Health = 5;
		} State;
	};
}