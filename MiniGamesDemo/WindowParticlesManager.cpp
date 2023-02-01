#include "WindowParticlesManager.h"

#include "Graphics/WindowManager.h"
#include "Math/Math.h"
#include "ImGui/ImGuiUtil.h"

#include <imgui.h>
#include <glm/gtc/random.hpp>

namespace DYE
{
	float WindowParticlesManager::WindowParticle::GetSize() const
	{
		float const sizeProgress = GetEasingFunction(SizeEaseType)(Timer / LifeTime);
		float const size = Math::Lerp(StartSize, EndSize, sizeProgress);
		return size;
	}

	void WindowParticlesManager::CircleEmitParticlesAt(glm::vec2 center, CircleEmitParams params)
	{
		for (int i = 0; i < params.NumberOfParticles; ++i)
		{
			bool foundAParticle = false;
			for (auto& particle : m_Particles)
			{
				if (particle.IsPlaying)
				{
					continue;
				}

				particle.Position = center;
				glm::vec2 direction = glm::circularRand(1.0f);
				if (params.HasFixedInitialVelocityDirectionY)
				{
					direction.y = params.FixedInitialVelocityDirectionY;
				}

				float const speed = glm::linearRand(params.InitialMinSpeed, params.InitialMaxSpeed);
				particle.Velocity = direction * speed;

				particle.LifeTime = params.LifeTime;
				particle.Gravity = params.Gravity;
				particle.DecelerationPerSecond = params.DecelerationPerSecond;
				particle.SizeEaseType = params.SizeEaseType;
				particle.StartSize = params.StartSize;
				particle.EndSize = params.EndSize;

				particle.IsPlaying = true;
				particle.Timer = 0.0f;

				if (m_ShowParticle)
				{
					particle.pWindow->SetPosition(particle.Position);
					particle.pWindow->SetSize(particle.StartSize, particle.StartSize);
					particle.pWindow->Raise();
				}

				foundAParticle = true;
				break;
			}

			if (foundAParticle)
			{
				continue;
			}

			// No available ripples in the pool, spawn a new one.
			WindowParticle newParticle;

			newParticle.Position = center;
			glm::vec2 direction = glm::circularRand(1.0f);
			if (params.HasFixedInitialVelocityDirectionY)
			{
				direction.y = params.FixedInitialVelocityDirectionY;
			}

			float const speed = glm::linearRand(params.InitialMinSpeed, params.InitialMaxSpeed);
			newParticle.Velocity = direction * speed;

			newParticle.LifeTime = params.LifeTime;
			newParticle.Gravity = params.Gravity;
			newParticle.DecelerationPerSecond = params.DecelerationPerSecond;
			newParticle.SizeEaseType = params.SizeEaseType;
			newParticle.StartSize = params.StartSize;
			newParticle.EndSize = params.EndSize;

			newParticle.IsPlaying = true;
			newParticle.Timer = 0.0f;

			newParticle.pWindow = WindowManager::CreateWindow(WindowProperty("Particle " + std::to_string(m_Particles.size()), 10, 10));
			newParticle.pWindow->SetBorderedIfWindowed(false);
			if (m_ShowParticle)
			{
				newParticle.pWindow->SetPosition(newParticle.Position);
				newParticle.pWindow->SetSize(newParticle.StartSize, newParticle.StartSize);
				newParticle.pWindow->Raise();
			}

			m_Particles.emplace_back(newParticle);
		}
	}

	void WindowParticlesManager::Initialize(int initialPoolSize)
	{
		for (int i = 0; i < initialPoolSize; ++i)
		{
			WindowParticle particle;
			particle.pWindow = WindowManager::CreateWindow(WindowProperty("Particle " + std::to_string(i), 0, 0));
			particle.pWindow->SetBorderedIfWindowed(false);

			// Hide the window outside the screen.
			particle.pWindow->SetPosition(-1920, -1080);

			m_Particles.emplace_back(particle);
		}
	}

	void WindowParticlesManager::ShowParticles()
	{
		m_ShowParticle = true;
	}

	void WindowParticlesManager::HideParticles()
	{
		m_ShowParticle = false;

		for (auto& particle : m_Particles)
		{
			// Hide the window outside the screen.
			particle.pWindow->SetPosition(-1920, -1080);
		}
	}

	void WindowParticlesManager::OnUpdate(float timeStep)
	{
		int updatedCount = 0;
		for (auto& particle : m_Particles)
		{
			if (!particle.IsPlaying)
			{
				continue;
			}
			updatedCount++;

			particle.Timer += timeStep;
			if (particle.Timer >= particle.LifeTime)
			{
				particle.Timer = particle.LifeTime;
				particle.IsPlaying = false;
			}

			if (particle.DecelerationPerSecond > 0.0f)
			{
				float const speed = glm::length(particle.Velocity);
				if (speed > 0.0f)
				{
					float newSpeed = speed - particle.DecelerationPerSecond * timeStep;
					if (newSpeed < 0.0f) newSpeed = 0.0f;
					particle.Velocity = glm::normalize(particle.Velocity) * newSpeed;
				}
			}

			particle.Velocity += timeStep * glm::vec2 {0, particle.Gravity};
			particle.Position += particle.Velocity * timeStep;

			float const sizeProgress = GetEasingFunction(particle.SizeEaseType)(particle.Timer / particle.LifeTime);
			float const size = Math::Lerp(particle.StartSize, particle.EndSize, sizeProgress);

			particle.pWindow->SetPosition(particle.Position);
			particle.pWindow->SetSize(size, size);

			if (!particle.IsPlaying)
			{
				// Hide the window outside the screen.
				particle.pWindow->SetPosition(-1920, -1080);
			}
		}
	}

	void WindowParticlesManager::Shutdown()
	{
		for (auto& particle : m_Particles)
		{
			WindowManager::CloseWindow(particle.pWindow->GetWindowID());
		}
	}

	void WindowParticlesManager::OnImGui()
	{
		if (ImGui::Begin("Window Particles Debugger"))
		{
			int updatedCount = 0;
			for (auto& particle : m_Particles)
			{
				if (particle.IsPlaying)
				{
					updatedCount++;
				}
			}

			ImGuiUtil::DrawReadOnlyTextWithLabel("Number Of Active Particles", std::to_string(updatedCount));
			for (auto& particle : m_Particles)
			{
				if (!particle.IsPlaying)
				{
					continue;
				}
				ImGui::Separator();
				ImGuiUtil::DrawReadOnlyTextWithLabel("Life Time", std::to_string(particle.Timer) + "/" + std::to_string(particle.LifeTime));
				ImGuiUtil::DrawReadOnlyTextWithLabel("Size", std::to_string(particle.GetSize()));
				ImGuiUtil::DrawVec2Control("Position", particle.Position);
			}
		}
		ImGui::End();
	}
}