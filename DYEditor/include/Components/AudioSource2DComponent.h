#pragma once

#include "Core/EditorProperty.h"
#include "Audio/AudioSource.h"
#include "Audio/AudioClip.h"

namespace DYE::DYEditor
{
    struct AudioSource2DComponent
    {
        // FIXME: in the future, each clip asset would have a import settings.
        AudioLoadType LoadType = AudioLoadType::DecompressOnLoad;

        DYE::AssetPath ClipAssetPath;
        AudioSource Source;
    };
}