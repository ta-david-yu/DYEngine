#pragma once

#include "Components/Transform.h"
#include "Components/Collider.h"

namespace DYE::MiniGame
{
	struct WindowFrame
	{
		Transform Transform;
		BoxCollider Collider;
	};
}