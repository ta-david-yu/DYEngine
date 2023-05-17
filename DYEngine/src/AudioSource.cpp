#include "Audio/AudioSource.h"

#include "Audio/AudioManager.h"
#include "Util/Macro.h"

#include <raudio.h>

namespace DYE
{
	AudioSource::AudioSource(const AudioSource &other) : m_Volume(other.m_Volume)
	{
		m_AudioClip = other.m_AudioClip;

		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				m_pNativeAudioDataBuffer = DYE_MALLOC(sizeof(Sound));
				*((Sound *) m_pNativeAudioDataBuffer) = LoadSoundFromWave(
					*(Wave *) m_AudioClip->GetNativeWaveDataPointer());
				break;
			}
			case AudioLoadType::Streaming:
			{
				m_pNativeAudioDataBuffer = DYE_MALLOC(sizeof(Music));
				*((Music *) m_pNativeAudioDataBuffer) = LoadMusicStream(m_AudioClip->GetPath().string().c_str());
				AudioManager::registerStreamingAudioSource(this);
				break;
			}
		}

		refreshNativeAudioDataVolume();
	}

	AudioSource::AudioSource(AudioSource &&other) noexcept
	{
		m_Volume = other.m_Volume;
		m_AudioClip = std::move(other.m_AudioClip);

		m_pNativeAudioDataBuffer = other.m_pNativeAudioDataBuffer;
		other.m_pNativeAudioDataBuffer = nullptr;

		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				break;
			}
			case AudioLoadType::Streaming:
			{
				AudioManager::registerStreamingAudioSource(this);
				break;
			}
		}
	}

	AudioSource::~AudioSource()
	{
		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				if (m_pNativeAudioDataBuffer != nullptr)
				{
					UnloadSound(*(Sound *) m_pNativeAudioDataBuffer);
				}
				break;
			}
			case AudioLoadType::Streaming:
			{
				AudioManager::unregisterStreamingAudioSource(this);
				if (m_pNativeAudioDataBuffer != nullptr)
				{
					UnloadMusicStream(*(Music *) m_pNativeAudioDataBuffer);
				}
				break;
			}
		}

		if (m_pNativeAudioDataBuffer != nullptr)
		{
			DYE_FREE(m_pNativeAudioDataBuffer);
		}
	}

	void AudioSource::SetClip(std::shared_ptr<AudioClip> audioClip)
	{
		if (m_AudioClip)
		{
			// There is already a clip set before.

			// If the previous clip is streaming load type,
			// we need to unregister this source from the audio system.

			switch (m_AudioClip->GetLoadType())
			{
				case AudioLoadType::DecompressOnLoad:
				{
					UnloadSound(*(Sound*) m_pNativeAudioDataBuffer);
					break;
				}
				case AudioLoadType::Streaming:
				{
					AudioManager::unregisterStreamingAudioSource(this);
					UnloadMusicStream(*(Music*) m_pNativeAudioDataBuffer);
					break;
				}
			}

			// We want to deallocate the buffer data first.
			DYE_FREE(m_pNativeAudioDataBuffer);
		}

		m_AudioClip = audioClip;

		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				m_pNativeAudioDataBuffer = DYE_MALLOC(sizeof(Sound));
				*((Sound*) m_pNativeAudioDataBuffer) = LoadSoundFromWave(*(Wave*) audioClip->GetNativeWaveDataPointer());
				break;
			}
			case AudioLoadType::Streaming:
			{
				m_pNativeAudioDataBuffer = DYE_MALLOC(sizeof(Music));
				*((Music*) m_pNativeAudioDataBuffer) = LoadMusicStream(audioClip->GetPath().string().c_str());
				AudioManager::registerStreamingAudioSource(this);
				break;
			}
		}

		refreshNativeAudioDataVolume();
	}

	void AudioSource::Play()
	{
		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioDataBuffer);
				PlaySound(sound);
				break;
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioDataBuffer);
				PlayMusicStream(music);
				break;
			}
		}
	}

	void AudioSource::Stop()
	{
		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioDataBuffer);
				StopSound(sound);
				break;
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioDataBuffer);
				StopMusicStream(music);
				break;
			}
		}
	}

	void AudioSource::Pause()
	{
		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioDataBuffer);
				PauseSound(sound);
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioDataBuffer);
				PauseMusicStream(music);
			}
		}
	}

	void AudioSource::Resume()
	{
		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioDataBuffer);
				ResumeSound(sound);
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioDataBuffer);
				ResumeMusicStream(music);
			}
		}
	}

	bool AudioSource::IsPlaying() const
	{
		if (!m_AudioClip)
		{
			return false;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioDataBuffer);
				return IsSoundPlaying(sound);
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioDataBuffer);
				return IsMusicStreamPlaying(music);
			}
		}
	}

	bool AudioSource::IsStreamLooping() const
	{
		if (!m_AudioClip)
		{
			return false;
		}

		return m_AudioClip->GetLoadType() == AudioLoadType::Streaming && ((Music*) m_pNativeAudioDataBuffer)->looping;
	}

	void AudioSource::SetStreamLooping(bool looping)
	{
		if (!m_AudioClip)
		{
			return;
		}

		if (m_AudioClip->GetLoadType() != AudioLoadType::Streaming)
		{
			return;
		}

		((Music*) m_pNativeAudioDataBuffer)->looping = looping;
	}

	float AudioSource::GetStreamTime() const
	{
		if (!m_AudioClip)
		{
			return 0;
		}

		if (m_AudioClip->GetLoadType() != AudioLoadType::Streaming)
		{
			return 0;
		}

		return GetMusicTimePlayed(*((Music*) m_pNativeAudioDataBuffer));
	}

	void AudioSource::SetStreamTime(float timeInSecond)
	{
		if (!m_AudioClip)
		{
			return;
		}

		if (m_AudioClip->GetLoadType() != AudioLoadType::Streaming)
		{
			return;
		}

		SeekMusicStream(*((Music*) m_pNativeAudioDataBuffer), timeInSecond);
	}

	void AudioSource::SetVolume(float volume)
	{
		m_Volume = volume;
		refreshNativeAudioDataVolume();
	}

	void AudioSource::refreshNativeAudioDataVolume()
	{
		if (!m_AudioClip)
		{
			return;
		}

		switch (m_AudioClip->GetLoadType())
		{
			case AudioLoadType::DecompressOnLoad:
			{
				Sound &sound = *((Sound*) m_pNativeAudioDataBuffer);
				SetSoundVolume(sound, m_Volume);
				break;
			}
			case AudioLoadType::Streaming:
			{
				Music &music = *((Music*) m_pNativeAudioDataBuffer);
				SetMusicVolume(music, m_Volume);
				break;
			}
		}
	}
}