#pragma once

#include <glm/glm.hpp>

namespace DYE::MiniGame
{
	struct PlayerPaddle;

	struct AttachableToPaddle
	{
		PlayerPaddle* AttachedPaddle = nullptr;
		glm::vec2 AttachOffset {0, 0};
	};
}