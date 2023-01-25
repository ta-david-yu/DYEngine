#include "Objects/PongBall.h"

#include "Objects/PlayerPaddle.h"

namespace DYE::MiniGame
{
	void PongBall::EquipToPaddle(PlayerPaddle &paddle, glm::vec2 offset)
	{
		Attachable.AttachToPaddle(paddle, offset);

		Transform.Position = paddle.Transform.Position + glm::vec3 {Attachable.AttachOffset, 0};
		Velocity.Value = {0, 0};
	}

	void PongBall::LaunchFromAttachedPaddle()
	{
		if (!Attachable.IsAttached)
		{
			return;
		}

		PlayerPaddle* paddle = Attachable.AttachedPaddle;
		auto paddleVelocity = paddle->VelocityBuffer;

		Velocity.Value = glm::vec3 {glm::normalize(Attachable.AttachOffset) * LaunchBaseSpeed + paddleVelocity, 0};
		Attachable.DetachFromCurrentPaddle();
	}
}