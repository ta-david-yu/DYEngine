#pragma once

namespace DYE
{
    class AudioSource;

    class AudioManager
    {
        friend AudioSource;
    public:
        static void Init();
        static void Close();

        static void UpdateRegisteredAudioStreams();

        static void DrawAudioManagerImGui(bool *pIsOpen = nullptr);

    private:
        static void registerStreamingAudioSource(AudioSource *pSource);
        static void unregisterStreamingAudioSource(AudioSource *pSource);
    };
}