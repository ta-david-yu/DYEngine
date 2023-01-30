#pragma once

#include "Components/Transform.h"
#include "Graphics/Texture.h"

namespace DYE::MiniGame
{
	struct SpriteUnsignedNumber
	{
	public:
		Transform Transform;
		float DigitDistanceOffset = 1.0f;
		float PopAnimationDuration = 0.5f;

		void LoadTexture();
		void SetValue(unsigned int value);
		unsigned int GetValue() const { return m_Value; }
		void PlayPopAnimation();

		void UpdateAnimation(float timeStep);
		void Render();

	private:
		unsigned int m_Value = 0;

		float m_PopAnimationTimer = 0.0f;

		std::vector<unsigned int> m_Digits;
		std::vector<std::shared_ptr<Texture2D>> m_DigitTextures;
	};
}