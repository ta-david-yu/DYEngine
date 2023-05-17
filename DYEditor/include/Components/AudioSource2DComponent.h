#pragma once

#include "Core/EditorProperty.h"
#include "Audio/AudioSource.h"
#include "Audio/AudioClip.h"

namespace DYE::DYEditor
{
	struct AudioSource2DComponent
	{
		DYE::AssetPath ClipAssetPath;
		std::shared_ptr<AudioClip> Clip;
		AudioSource Source;
	};
}