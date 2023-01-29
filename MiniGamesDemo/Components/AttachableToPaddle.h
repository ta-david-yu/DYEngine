#pragma once

#include <glm/glm.hpp>

namespace DYE::MiniGame
{
	struct PlayerPaddle;

	struct AttachableToPaddle
	{
		bool IsAttached = false;
		PlayerPaddle* AttachedPaddle = nullptr;
		glm::vec2 AttachOffset {0, 0};

		void AttachToPaddle(PlayerPaddle& paddle, glm::vec2 offset);
		void DetachFromCurrentPaddle();
	};
}