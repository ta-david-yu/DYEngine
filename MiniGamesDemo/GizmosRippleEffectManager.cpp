#include "GizmosRippleEffectManager.h"

#include "Math/Math.h"
#include "Graphics/DebugDraw.h"

namespace DYE
{
	float GizmosRippleEffectManager::RippleInstance::GetCurrentRadius() const
	{
		float const mappedT = GetEasingFunction(EaseType)(Timer / LifeTime);
		float const radius = Math::Lerp(StartRadius, EndRadius, mappedT);
		return radius;
	}

	glm::vec4 GizmosRippleEffectManager::RippleInstance::GetCurrentColor() const
	{
		return Math::Lerp(StartColor, EndColor, Timer / LifeTime);
	}

	void GizmosRippleEffectManager::SpawnRippleAt(glm::vec2 center, RippleEffectParameters params)
	{
		for (auto& ripple : m_Ripples)
		{
			if (ripple.IsPlaying)
			{
				continue;
			}

			ripple.Center = center;
			ripple.EaseType = params.EaseType;
			ripple.LifeTime = params.LifeTime;
			ripple.StartRadius = params.StartRadius;
			ripple.EndRadius = params.EndRadius;
			ripple.StartColor = params.StartColor;
			ripple.EndColor = params.EndColor;

			ripple.IsPlaying = true;
			ripple.Timer = 0.0f;
			return;
		}

		// No available ripples in the pool, spawn a new one.
		RippleInstance newRipple;
		newRipple.Center = center;
		newRipple.EaseType = params.EaseType;
		newRipple.LifeTime = params.LifeTime;
		newRipple.StartRadius = params.StartRadius;
		newRipple.EndRadius = params.EndRadius;
		newRipple.StartColor = params.StartColor;
		newRipple.EndColor = params.EndColor;

		newRipple.IsPlaying = true;
		newRipple.Timer = 0.0f;

		m_Ripples.emplace_back(newRipple);
	}

	void GizmosRippleEffectManager::OnUpdate(float timeStep)
	{
		for (auto& ripple : m_Ripples)
		{
			if (!ripple.IsPlaying)
			{
				continue;
			}

			ripple.Timer += timeStep;
			if (ripple.Timer >= ripple.LifeTime)
			{
				ripple.Timer = ripple.LifeTime;
				ripple.IsPlaying = false;
			}

			float const radius = ripple.GetCurrentRadius();
			auto const color = ripple.GetCurrentColor();
			DebugDraw::Circle(glm::vec3 {ripple.Center, 0}, radius, {0, 0, 1}, color);
		}
	}
}