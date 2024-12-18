#pragma once

#include "Audio/AudioClip.h"

#include <memory>

namespace DYE
{
    class AudioSource
    {
    public:
        AudioSource() = default;
        AudioSource(AudioSource const &other);
        AudioSource(AudioSource &&other) noexcept;
        ~AudioSource();

        void SetClip(std::shared_ptr<AudioClip> audioClip);
        AudioClip *GetClip() const { return m_AudioClip.get(); }

        void Play();
        void Stop();
        void Pause();
        void Resume();
        bool IsPlaying() const;
        bool IsStreamLooping() const;
        void SetStreamLooping(bool looping);
        float GetStreamTime() const;
        void SetStreamTime(float timeInSecond);
        inline float GetVolume() const { return m_Volume; }
        void SetVolume(float volume);

        inline void *GetNativeAudioDataBuffer() const { return m_pNativeAudioDataBuffer; }

    private:
        void refreshNativeAudioDataVolume();

    private:
        void *m_pNativeAudioDataBuffer = nullptr;
        float m_Volume = 1;
        std::shared_ptr<AudioClip> m_AudioClip;
    };
}