#pragma once

namespace DYE
{
	class AudioClip;

	class AudioSystem
	{
		friend AudioClip;
	public:
		static void Init();
		static void Close();

		static void UpdateRegisteredAudioStreams();

	private:
		static void registerAudioClip(AudioClip *pAudioClip);
		static void unregisterAudioClip(AudioClip *pAudioClip);
	};
}