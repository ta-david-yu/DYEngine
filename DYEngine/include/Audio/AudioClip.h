#pragma once

#include <memory>
#include <filesystem>

namespace DYE
{
	class AudioSystem;

	enum class AudioLoadType
	{
		DecompressOnLoad,
		Streaming
	};

	struct AudioClipProperties
	{
		AudioLoadType LoadType = AudioLoadType::DecompressOnLoad;
	};

	class AudioClip
	{
		friend AudioSystem;
	public:

		static std::shared_ptr<AudioClip> Create(std::filesystem::path const& path, AudioClipProperties properties);

		/// Don't call this. Call AudioClip::Create instead.
		AudioClip() = default;
		~AudioClip();

		void Play();
		void Stop();
		void Pause();
		bool IsPlaying();
		bool IsStreamLooping();
		void SetStreamLooping(bool looping);

		AudioLoadType GetLoadType() const { return m_Properties.LoadType; }
		auto GetPath() const -> std::filesystem::path { return m_Path; }

	private:
		void* m_pNativeAudioClip;
		AudioClipProperties m_Properties;
		std::filesystem::path m_Path {};
	};
}