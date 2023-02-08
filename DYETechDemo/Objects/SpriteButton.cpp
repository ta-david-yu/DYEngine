#include "Objects/SpriteButton.h"

namespace DYE::MiniGame
{
	void SpriteButton::SetSelectAppearance()
	{
		Sprite.Texture = SelectedTexture;
	}

	void SpriteButton::SetDeselectAppearance()
	{
		Sprite.Texture = DeselectedTexture;
	}
}