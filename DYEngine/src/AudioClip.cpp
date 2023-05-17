#include "Audio/AudioClip.h"

#include "Util/Macro.h"

#include <raudio.h>

namespace DYE
{
	std::shared_ptr<AudioClip> AudioClip::Create(const std::filesystem::path &path, AudioClipProperties properties)
	{
		DYE_LOG("<< Start creating clip from \"%s\" -", path.string().c_str());

		std::shared_ptr<AudioClip> audioClip = std::make_shared<AudioClip>();

		audioClip->m_Properties = properties;
		audioClip->m_Path = path;

		switch (properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				audioClip->m_pNativeWaveData = DYE_MALLOC(sizeof(Wave));
				*((Wave*) audioClip->m_pNativeWaveData) = LoadWave(path.string().c_str());

				Wave &wave = *(Wave*) audioClip->m_pNativeWaveData;
				audioClip->m_Length = (float) wave.frameCount / wave.sampleRate;

				break;
			}
			case AudioLoadType::Streaming:
			{
				// We don't preload wave data if the load type is streaming.
				audioClip->m_pNativeWaveData = nullptr;

				// In order to get the length, we need to load the stream.
				Music music = LoadMusicStream(audioClip->m_Path.string().c_str());
				audioClip->m_Length = GetMusicTimeLength(music);

				DYE_LOG("Length: %f", audioClip->m_Length);
				DYE_LOG("FrameCount: %d", music.frameCount);
				DYE_LOG("SampleRate: %d", music.stream.sampleRate);


				UnloadMusicStream(music);
				break;
			}
		}

		printf("\n");
		DYE_LOG("- End create clip \"%s\" >>", path.string().c_str());

		return std::move(audioClip);
	}

	AudioClip::~AudioClip()
	{
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				UnloadWave(*((Wave*) m_pNativeWaveData));
				break;
			}
			case AudioLoadType::Streaming:
			{
				break;
			}
		}
		DYE_FREE(m_pNativeWaveData);
	}

	float AudioClip::GetLength() const
	{

		return m_Length;
	}
}