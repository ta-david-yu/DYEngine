#include "Objects/PongBall.h"

#include "Objects/PlayerPaddle.h"
#include "Math/Math.h"
#include "Math/EasingFunctions.h"

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

	void PongBall::PlayRespawnAnimation()
	{
		m_RespawnAnimationTimer = RespawnAnimationDuration;
		Transform.Scale = {0, 0, 0};
	}

	void PongBall::PlayHitAnimation()
	{
		m_HitAnimationTimer = HitAnimationDuration;
		Transform.Scale = {1.5f, 1.5f, 1.5f};
	}

	void PongBall::PlayGoalAnimation()
	{
		m_GoalAnimationTimer = GoalAnimationDuration;
		Transform.Scale = {1.0f, 1.0f, 1.0f};
	}

	void PongBall::UpdateAnimation(float timeStep)
	{
		if (m_RespawnAnimationTimer > 0.0f)
		{
			m_RespawnAnimationTimer -= timeStep;
			if (m_RespawnAnimationTimer <= 0.0f)
			{
				m_RespawnAnimationTimer = 0.0f;
			}

			float const t = EaseOutBackDouble((RespawnAnimationDuration - m_RespawnAnimationTimer) / RespawnAnimationDuration);
			float const scale = Math::Lerp(0, 1, t);
			Transform.Scale = {scale, scale, scale};
			return;
		}

		if (m_HitAnimationTimer > 0.0f)
		{
			m_HitAnimationTimer -= timeStep;
			if (m_HitAnimationTimer <= 0.0f)
			{
				m_HitAnimationTimer = 0.0f;
			}

			float const t = EaseOutCubic((HitAnimationDuration - m_HitAnimationTimer) / HitAnimationDuration);
			float const scale = Math::Lerp(1.5f, 1.0f, t);
			Transform.Scale = {scale, scale, scale};
			return;
		}

		if (m_GoalAnimationTimer > 0.0f)
		{
			m_GoalAnimationTimer -= timeStep;
			if (m_GoalAnimationTimer <= 0.0f)
			{
				m_GoalAnimationTimer = 0.0f;
			}

			float const t = EaseInCubic((GoalAnimationDuration - m_GoalAnimationTimer) / GoalAnimationDuration);
			float const scale = Math::Lerp(1.0f, 0.0f, t);
			Transform.Scale = {scale, scale, scale};
			return;
		}
	}
}