#pragma once

#include "Graphics/Camera.h"

namespace DYE::DYEntity
{
	struct CameraComponent
	{
		bool IsEnabled = true;
		CameraProperties Properties;
	};
}