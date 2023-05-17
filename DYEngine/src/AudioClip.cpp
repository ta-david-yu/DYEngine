#include "Audio/AudioClip.h"

#include "Audio/AudioSystem.h"

#include <cstdlib>
#include <raudio.h>

namespace DYE
{
	std::shared_ptr<AudioClip> AudioClip::Create(const std::filesystem::path &path, AudioClipProperties properties)
	{
		std::shared_ptr<AudioClip> audioClip = std::make_shared<AudioClip>();
		audioClip->m_Path = path;
		audioClip->m_Properties = properties;

		switch (audioClip->m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				audioClip->m_pNativeAudioClip = std::malloc(sizeof(Sound));
				*((Sound*) audioClip->m_pNativeAudioClip) = LoadSound(path.string().c_str());
				break;
			}
			case AudioLoadType::Streaming:
			{
				audioClip->m_pNativeAudioClip = std::malloc(sizeof(Music));
				*((Music*) audioClip->m_pNativeAudioClip) = LoadMusicStream(path.string().c_str());
				AudioSystem::registerAudioClip(audioClip.get());
				break;
			}
		}

		return std::move(audioClip);
	}

	AudioClip::~AudioClip()
	{
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				UnloadSound(*((Sound*) m_pNativeAudioClip));
				break;
			}
			case AudioLoadType::Streaming:
			{
				AudioSystem::unregisterAudioClip(this);
				UnloadMusicStream(*((Music*) m_pNativeAudioClip));
				break;
			}
		}
		std::free(m_pNativeAudioClip);
	}

	void AudioClip::Play()
	{
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioClip);
				PlaySound(sound);
				break;
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioClip);
				PlayMusicStream(music);
				break;
			}
		}
	}

	void AudioClip::Stop()
	{
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioClip);
				StopSound(sound);
				break;
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioClip);
				StopMusicStream(music);
				break;
			}
		}
	}

	void AudioClip::Pause()
	{
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioClip);
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioClip);
			}
		}
	}

	bool AudioClip::IsPlaying()
	{
		switch (m_Properties.LoadType)
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioClip);
				return IsSoundPlaying(sound);
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioClip);
				return IsMusicStreamPlaying(music);
			}
		}
	}

	bool AudioClip::IsStreamLooping()
	{
		return m_Properties.LoadType == AudioLoadType::Streaming && ((Music*) m_pNativeAudioClip)->looping;
	}

	void AudioClip::SetStreamLooping(bool looping)
	{
		if (m_Properties.LoadType != AudioLoadType::Streaming)
		{
			return;
		}

		((Music*) m_pNativeAudioClip)->looping = looping;
	}
}