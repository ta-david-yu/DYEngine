#include "Audio/AudioManager.h"

#include "Audio/AudioSource.h"
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
        std::vector<AudioSource *> RegisteredSources;
    };

    static AudioStreamManagerData s_Data;

    void AudioManager::Init()
    {
        InitAudioDevice();
    }

    void AudioManager::Close()
    {
        CloseAudioDevice();
    }

    void AudioManager::UpdateRegisteredAudioStreams()
    {
        std::vector<AudioSource *> &registeredSources = s_Data.RegisteredSources;
        for (AudioSource *pSource: registeredSources)
        {
            if (!pSource->IsPlaying())
            {
                continue;
            }

            if (pSource->GetClip()->GetLoadType() != AudioLoadType::Streaming)
            {
                DYE_LOG("AudioSource's Clip '%s' is not loaded as a streaming audio but is registered to AudioStreamManager."
                        "This should probably never happen but did for some reasons :P",
                        pSource->GetClip()->GetPath().string().c_str());
                continue;
            }

            Music music = *((Music *) pSource->GetNativeAudioDataBuffer());
            UpdateMusicStream(music);
        }
    }

    void AudioManager::DrawAudioManagerImGui(bool *pIsOpen)
    {
        // Set a default size for the window in case it has never been opened before.
        const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20),
                                ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Audio Manager", pIsOpen))
        {
            ImGui::End();
            return;
        }

        const float windowWidth = 175;
        for (int i = 0; i < s_Data.RegisteredSources.size(); ++i)
        {
            AudioSource &audioSource = *s_Data.RegisteredSources[i];

            auto const originalControlLabelWidth = ImGuiUtil::Settings::ControlLabelWidth;
            ImGuiUtil::Settings::ControlLabelWidth = 100;

            ImGui::BeginGroup();
            char id[32] = "";
            sprintf(id, "AudioStream%d", i);
            ImGui::BeginChild(id, ImVec2(0, 120), true, ImGuiWindowFlags_HorizontalScrollbar);

            ImGuiUtil::DrawReadOnlyTextWithLabel("Path", audioSource.GetClip()->GetPath().string().c_str());
            ImGuiUtil::DrawReadOnlyTextWithLabel("Length (sec)", std::to_string(audioSource.GetClip()->GetLength()));

            bool isLooping = audioSource.IsStreamLooping();
            if (ImGuiUtil::DrawBoolControl("Is Looping", isLooping))
            {
                audioSource.SetStreamLooping(isLooping);
            }

            ImGui::Separator();
            bool const isPlaying = audioSource.IsPlaying();
            ImGuiUtil::DrawReadOnlyTextWithLabel("Is Playing", isPlaying ? "True" : "False");

            if (isPlaying)
            {
                float playTime = audioSource.GetStreamTime();
                float const fullLength = audioSource.GetClip()->GetLength();
                if (ImGui::SliderFloat("##ProgressBar", &playTime, 0, audioSource.GetClip()->GetLength()))
                {
                    audioSource.SetStreamTime(playTime);
                }
            }

            ImGui::EndChild();
            ImGui::EndGroup();

            ImGuiUtil::Settings::ControlLabelWidth = originalControlLabelWidth;
        }
        ImGui::End();
    }

    void AudioManager::registerStreamingAudioSource(AudioSource *pSource)
    {
        DYE_ASSERT(pSource->GetClip()->GetLoadType() == AudioLoadType::Streaming);

        auto findItr = std::find_if(s_Data.RegisteredSources.begin(), s_Data.RegisteredSources.end(),
                                    [pSource](AudioSource *pSourceElement)
                                    {
                                        return pSourceElement == pSource;
                                    });

        if (findItr != s_Data.RegisteredSources.end())
        {
            DYE_LOG("AudioSource with the clip '%s' has already been registered. Skip it!", pSource->GetClip()->GetPath().string().c_str());
            return;
        }

        s_Data.RegisteredSources.push_back(pSource);
    }

    void AudioManager::unregisterStreamingAudioSource(AudioSource *pSource)
    {
        DYE_ASSERT(pSource->GetClip()->GetLoadType() == AudioLoadType::Streaming);

        std::vector<AudioSource *> &registeredSources = s_Data.RegisteredSources;
        for (int i = 0; i < registeredSources.size(); i++)
        {
            if (registeredSources[i] == pSource)
            {
                registeredSources.erase(registeredSources.begin() + i);
                break;
            }
        }
//
//		std::erase_if(s_Data.RegisteredSources, [pSource](AudioSource* pSourceElement)
//		{
//			return pSourceElement == pSource;
//		});
    }
}