#include "Audio/AudioClip.h"

#include "Util/Macro.h"

#include <raudio.h>

namespace DYE
{
	std::shared_ptr<AudioClip> AudioClip::Create(const std::filesystem::path &path, AudioClipProperties properties)
	{
		std::shared_ptr<AudioClip> audioClip = std::make_shared<AudioClip>();

		switch (properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				audioClip->m_pNativeWaveData = DYE_MALLOC(sizeof(Wave));
				*((Wave*) audioClip->m_pNativeWaveData) = LoadWave(path.string().c_str());
				break;
			}
			case AudioLoadType::Streaming:
			{
				// We don't preload wave data if the load type is streaming.
				audioClip->m_pNativeWaveData = nullptr;
				break;
			}
		}

		audioClip->m_Properties = properties;
		audioClip->m_Path = path;

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
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Wave &wave = *(Wave*) m_pNativeWaveData;
				return (float) wave.frameCount / wave.sampleRate;
			}
			case AudioLoadType::Streaming:
			{
				Music music = LoadMusicStream(m_Path.string().c_str());
				float length = GetMusicTimeLength(music);
				UnloadMusicStream(music);
				return length;
			}
		}
	}
}