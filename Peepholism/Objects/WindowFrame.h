#pragma once

#include "Components/Transform.h"
#include "Components/Collider.h"

namespace DYE::Pong
{
	struct WindowFrame
	{
		Transform Transform;
		BoxCollider Collider;
	};
}