#include "Objects/SpriteUnsignedNumber.h"

#include "Graphics/RenderPipelineManager.h"
#include "Graphics/RenderPipeline2D.h"

namespace DYE::MiniGame
{
	void SpriteUnsignedNumber::LoadTexture()
	{
		m_DigitTextures.clear();

		for (int i = 0; i < 10; i++)
		{
			std::filesystem::path const fileName = "assets\\Sprite_Digit_" + std::to_string(i) + ".png";
			auto texture = Texture2D::Create(fileName);
			texture->PixelsPerUnit = 32;
			m_DigitTextures.emplace_back(texture);
		}
	}

	void SpriteUnsignedNumber::SetValue(unsigned int value)
	{
		m_Value = value;
		m_Digits.clear();

		int number = static_cast<int>(m_Value);
		int numberOfDigits = 0;
		while (number)
		{
			int digit = number % 10;
			number /= 10;
			numberOfDigits++;
			m_Digits.push_back(digit);
		}
	}

	void SpriteUnsignedNumber::Render()
	{
		glm::mat4 modelMatrix = glm::mat4 {1.0f};
		modelMatrix = glm::translate(modelMatrix, Transform.Position);
		modelMatrix = modelMatrix * glm::toMat4(Transform.Rotation);
		modelMatrix = glm::scale(modelMatrix, Transform.Scale);

		if (m_Digits.empty())
		{
			RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
				->SubmitSprite(m_DigitTextures[0], {1, 1, 1, 1}, modelMatrix);
			return;
		}

		int const offsetCount = m_Digits.size() - 1;
		float const startX = -(offsetCount * DigitDistanceOffset) * 0.5f;

		for (int i = 0; i < m_Digits.size(); ++i)
		{
			int const digitIndex = m_Digits.size() - i - 1;
			unsigned int const digit = m_Digits[digitIndex];

			glm::vec3 const digitPosition = {startX + i * DigitDistanceOffset, 0, 0};
			glm::mat4 const digitModelMatrix = glm::translate(modelMatrix, digitPosition);

			RenderPipelineManager::GetTypedActiveRenderPipelinePtr<RenderPipeline2D>()
				->SubmitSprite(m_DigitTextures[digit], {1, 1, 1, 1}, digitModelMatrix);
		}
	}
}