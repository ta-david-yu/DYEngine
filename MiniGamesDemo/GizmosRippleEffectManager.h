#pragma once

#include "Math/EasingFunctions.h"

#include <glm/glm.hpp>
#include <vector>

namespace DYE
{
	struct RippleEffectParameters
	{
		Ease EaseType = Ease::EaseOutCubic;
		float LifeTime = 1;
		float StartRadius = 1.0f;
		float EndRadius = 2.0f;
		glm::vec4 StartColor {1, 1, 1, 1};
		glm::vec4 EndColor {1, 1, 1, 0};
	};

	class GizmosRippleEffectManager
	{
	private:
		struct RippleInstance
		{
			bool IsPlaying = false;
			float Timer = 0.0f;
			float GetCurrentRadius() const;
			glm::vec4 GetCurrentColor() const;

			glm::vec2 Center;
			Ease EaseType = Ease::EaseOutCubic;
			float LifeTime = 1.0f;
			float StartRadius = 1.0f;
			float EndRadius = 1.0f;
			glm::vec4 StartColor {1, 1, 1, 1};
			glm::vec4 EndColor {1, 1, 1, 0};
		};

		std::vector<RippleInstance> m_Ripples;

	public:
		void SpawnRippleAt(glm::vec2 center, RippleEffectParameters params);
		void OnUpdate(float timeStep);
	};
}