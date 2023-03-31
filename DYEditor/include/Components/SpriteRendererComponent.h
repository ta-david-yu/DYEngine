#pragma once

#include "Core/EditorProperty.h"
#include "Math/Color.h"
#include "Graphics/Texture.h"

namespace DYE::DYEditor
{
	struct SpriteRendererComponent
	{
		bool IsEnabled = true;
		glm::vec4 Color = Color::White;
		DYE::FilePath TextureAssetPath;
		std::shared_ptr<Texture2D> Texture = Texture2D::GetDefaultTexture();
	};
}