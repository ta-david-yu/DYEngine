#pragma once

#include "Audio/AudioClip.h"

#include <memory>

namespace DYE
{
	class AudioSource
	{
	public:
		AudioSource() = default;
		AudioSource(AudioSource const& other);
		~AudioSource();

		void SetClip(std::shared_ptr<AudioClip> audioClip);
		AudioClip *GetClip() const { return m_AudioClip.get(); }

		void Play();
		void Stop();
		void Pause();
		bool IsPlaying() const;
		bool IsStreamLooping() const;
		void SetStreamLooping(bool looping);
		float GetStreamTime() const;
		void SetStreamTime(float timeInSecond);

		inline void *GetNativeAudioDataBuffer() const { return m_pNativeAudioDataBuffer; }

	private:
		void *m_pNativeAudioDataBuffer = nullptr;
		std::shared_ptr<AudioClip> m_AudioClip;
	};
}