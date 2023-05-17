#include "Audio/AudioSystem.h"

#include "Audio/AudioClip.h"
#include "Util/Macro.h"
#include "Util/Logger.h"

#include <vector>
#include <algorithm>
#include <raudio.h>

namespace DYE
{
	struct AudioStreamManagerData
	{
		std::vector<AudioClip*> RegisteredClips;
	};

	static AudioStreamManagerData s_Data;

	void AudioSystem::Init()
	{
		InitAudioDevice();
	}

	void AudioSystem::Close()
	{
		CloseAudioDevice();
	}

	void AudioSystem::UpdateRegisteredAudioStreams()
	{
		for (AudioClip *pClip : s_Data.RegisteredClips)
		{
			if (!pClip->IsPlaying())
			{
				continue;
			}

			if (pClip->GetLoadType() != AudioLoadType::Streaming)
			{
				DYE_LOG("AudioClip '%s' is not loaded as a streaming audio but is registered to AudioStreamManager."
						"This should probably never happen but did for some reasons :P",
						pClip->GetPath().string().c_str());
				continue;
			}

			Music music = *((Music*) pClip->m_pNativeAudioClip);
			UpdateMusicStream(music);
		}
	}

	void AudioSystem::registerAudioClip(AudioClip *pAudioClip)
	{
		DYE_ASSERT(pAudioClip->GetLoadType() == AudioLoadType::Streaming);

		auto findItr = std::find_if(s_Data.RegisteredClips.begin(), s_Data.RegisteredClips.end(),
					 [pAudioClip](AudioClip* pClipElement)
					 {
						return pClipElement == pAudioClip;
					 });

		if (findItr != s_Data.RegisteredClips.end())
		{
			DYE_LOG("AudioClip '%s' has already been registered. Skip it!", pAudioClip->GetPath().string().c_str());
			return;
		}

		s_Data.RegisteredClips.push_back(pAudioClip);
	}

	void AudioSystem::unregisterAudioClip(AudioClip *pAudioClip)
	{
		DYE_ASSERT(pAudioClip->GetLoadType() == AudioLoadType::Streaming);

		std::erase_if(s_Data.RegisteredClips, [pAudioClip](AudioClip *pClipElement)
		{
			return pClipElement == pAudioClip;
		});
	}
}