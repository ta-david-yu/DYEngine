#pragma once

#include "Graphics/WindowBase.h"
#include "Math/EasingFunctions.h"

#include <vector>
#include <glm/glm.hpp>

namespace DYE
{
	struct CircleEmitParams
	{
		int NumberOfParticles = 2;
		float LifeTime = 1.0f;
		float Gravity = 50;
		Ease SizeEaseType = Ease::EaseInCubic;
		float StartSize = 50;
		float EndSize = 0;
		float InitialMinSpeed = 50;
		float InitialMaxSpeed = 75;
	};

	// Screen space window particles manager
	class WindowParticlesManager
	{
	private:
		struct WindowParticle
		{
			bool IsPlaying = false;
			float Timer = 0.0f;
			glm::vec2 Velocity;
			glm::vec2 Position;

			float LifeTime = 1.0f;
			float Gravity = 50;
			Ease SizeEaseType = Ease::EaseInCubic;
			float StartSize = 50;
			float EndSize = 0;

			float GetSize() const;

			WindowBase* pWindow;
		};

		constexpr static int InitialPoolSize = 20;
		std::vector<WindowParticle> m_Particles;
		bool m_ShowParticle = false;

	public:
		void CircleEmitParticlesAt(glm::vec2 center, CircleEmitParams params);

		void ShowParticles();
		void HideParticles();

		void Initialize();
		void OnUpdate(float timeStep);
		void Shutdown();

		void OnImGui();
	};
}