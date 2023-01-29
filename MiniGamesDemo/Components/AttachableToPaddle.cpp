#include "Components/AttachableToPaddle.h"

#include "Objects/PlayerPaddle.h"

namespace DYE::MiniGame
{
	void AttachableToPaddle::AttachToPaddle(PlayerPaddle& paddle, glm::vec2 offset)
	{
		IsAttached = true;
		AttachedPaddle = &paddle;
		AttachOffset = offset;
	}

	void AttachableToPaddle::DetachFromCurrentPaddle()
	{
		IsAttached = false;
		AttachedPaddle = nullptr;
		AttachOffset = {0, 0};
	}
}