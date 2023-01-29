#pragma once

#include "Components/Transform.h"
#include "Components/Velocity.h"
#include "Components/Collider.h"
#include "Components/Sprite.h"
#include "Components/Hittable.h"
#include "Components/AttachableToPaddle.h"

namespace DYE::MiniGame
{
	struct PongBall
	{
	public:
		Transform Transform;
		Velocity Velocity;
		CircleCollider Collider;
		Hittable Hittable;
		AttachableToPaddle Attachable;
		Sprite Sprite;
		float LaunchBaseSpeed;

		float RespawnAnimationDuration = 0.5f;
		float HitAnimationDuration = 0.25f;
		float GoalAnimationDuration = 0.25f;

		void EquipToPaddle(PlayerPaddle& paddle, glm::vec2 offset);
		void LaunchFromAttachedPaddle();

		void PlayRespawnAnimation();
		void PlayHitAnimation();
		void PlayGoalAnimation();

		void UpdateAnimation(float timeStep);

	private:
		float m_RespawnAnimationTimer = 0.0f;
		float m_HitAnimationTimer = 0.0f;
		float m_GoalAnimationTimer = 0.0f;
	};
}