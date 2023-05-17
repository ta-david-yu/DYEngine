#include "Audio/AudioSystem.h"

#include "Audio/AudioClip.h"
#include "Util/Macro.h"
#include "Util/Logger.h"
#include "ImGui/ImGuiUtil.h"

#include <vector>
#include <algorithm>
#include <raudio.h>

namespace DYE
{
	struct AudioStreamManagerData
	{
		std::vector<AudioClip*> RegisteredClips;
	};

	static AudioStreamManagerData s_Data;

	void AudioSystem::Init()
	{
		InitAudioDevice();
	}

	void AudioSystem::Close()
	{
		CloseAudioDevice();
	}

	void AudioSystem::UpdateRegisteredAudioStreams()
	{
		for (AudioClip *pClip : s_Data.RegisteredClips)
		{
			if (!pClip->IsPlaying())
			{
				continue;
			}

			if (pClip->GetLoadType() != AudioLoadType::Streaming)
			{
				DYE_LOG("AudioClip '%s' is not loaded as a streaming audio but is registered to AudioStreamManager."
						"This should probably never happen but did for some reasons :P",
						pClip->GetPath().string().c_str());
				continue;
			}

			Music music = *((Music*) pClip->m_pNativeAudioClip);
			UpdateMusicStream(music);
		}
	}

	void AudioSystem::DrawAudioSystemImGui(bool *pIsOpen)
	{
		// Set a default size for the window in case it has never been opened before.
		const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
								ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

		if (!ImGui::Begin("Audio System", pIsOpen))
		{
			ImGui::End();
			return;
		}

		const float windowWidth = 175;
		for (int i = 0; i < s_Data.RegisteredClips.size(); ++i)
		{
			AudioClip &audioClip = *s_Data.RegisteredClips[i];

			auto const originalControlLabelWidth = ImGuiUtil::Settings::ControlLabelWidth;
			ImGuiUtil::Settings::ControlLabelWidth = 100;

			ImGui::BeginGroup();
			char id[32] = "";
			sprintf(id, "AudioStream%d", i);
			ImGui::BeginChild(id, ImVec2(0, 120), true, ImGuiWindowFlags_HorizontalScrollbar);

			ImGuiUtil::DrawReadOnlyTextWithLabel("Path", audioClip.GetPath().string().c_str());
			ImGuiUtil::DrawReadOnlyTextWithLabel("Length (sec)", std::to_string(audioClip.GetLength()));

			bool isLooping = audioClip.IsStreamLooping();
			if (ImGuiUtil::DrawBoolControl("Is Looping", isLooping))
			{
				audioClip.SetStreamLooping(isLooping);
			}

			ImGui::Separator();
			bool const isPlaying = audioClip.IsPlaying();
			ImGuiUtil::DrawReadOnlyTextWithLabel("Is Playing", isPlaying? "True" : "False");

			if (isPlaying)
			{
				float playTime = audioClip.GetStreamPlayedTime();
				if (ImGui::SliderFloat("##ProgressBar", &playTime, 0, audioClip.GetLength()))
				{
					audioClip.SetStreamTime(playTime);
				}
			}

			ImGui::EndChild();
			ImGui::EndGroup();

			ImGuiUtil::Settings::ControlLabelWidth = originalControlLabelWidth;
		}
		ImGui::End();
	}

	void AudioSystem::registerAudioClip(AudioClip *pAudioClip)
	{
		DYE_ASSERT(pAudioClip->GetLoadType() == AudioLoadType::Streaming);

		auto findItr = std::find_if(s_Data.RegisteredClips.begin(), s_Data.RegisteredClips.end(),
					 [pAudioClip](AudioClip* pClipElement)
					 {
						return pClipElement == pAudioClip;
					 });

		if (findItr != s_Data.RegisteredClips.end())
		{
			DYE_LOG("AudioClip '%s' has already been registered. Skip it!", pAudioClip->GetPath().string().c_str());
			return;
		}

		s_Data.RegisteredClips.push_back(pAudioClip);
	}

	void AudioSystem::unregisterAudioClip(AudioClip *pAudioClip)
	{
		DYE_ASSERT(pAudioClip->GetLoadType() == AudioLoadType::Streaming);

		std::erase_if(s_Data.RegisteredClips, [pAudioClip](AudioClip *pClipElement)
		{
			return pClipElement == pAudioClip;
		});
	}
}