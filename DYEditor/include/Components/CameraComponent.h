#pragma once

#include "Graphics/Camera.h"

namespace DYE::DYEditor
{
	struct CameraComponent
	{
		bool IsEnabled = true;
		CameraProperties Properties;
	};
}