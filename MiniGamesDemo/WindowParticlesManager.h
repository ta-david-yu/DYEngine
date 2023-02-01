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
		float LifeTime = 0.75f;
		float Gravity = 2500;
		Ease SizeEaseType = Ease::EaseInCubic;
		float StartSize = 25;
		float EndSize = 5;
		float InitialMinSpeed = 550;
		float InitialMaxSpeed = 750;
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
			float EndSize = 50;

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