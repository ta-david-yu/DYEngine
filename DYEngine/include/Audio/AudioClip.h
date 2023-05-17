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

		float GetLength() const;
		AudioLoadType GetLoadType() const { return m_Properties.LoadType; }
		auto GetPath() const -> std::filesystem::path { return m_Path; }

		// Return null if the load type is Streaming.
		void *GetNativeWaveDataPointer() const { return m_pNativeWaveData; }

	private:
		// This is only valid when the load type is DecompressOnLoad.
		void* m_pNativeWaveData = nullptr;
		AudioClipProperties m_Properties;
		std::filesystem::path m_Path {};
	};
}