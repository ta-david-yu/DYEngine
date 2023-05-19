#include "Systems/AudioSystems.h"

#include "Components/AudioSource2DComponent.h"
#include "Components/Command/StartAudioSourceComponent.h"
#include "Components/Command/StopAudioSourceComponent.h"
#include "Components/PlayAudioSourceOnInitializeComponent.h"
#include "Core/Entity.h"

namespace DYE::DYEditor
{
	void AudioSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		// Start audio source.
		{
			auto startView = world.GetView<AudioSource2DComponent, StartAudioSourceComponent>();
			for (auto entity: startView)
			{
				auto &audioSource = startView.get<AudioSource2DComponent>(entity);
				audioSource.Source.Play();

				auto wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				wrappedEntity.RemoveComponent<StartAudioSourceComponent>();
			}
		}

		// Stop audio source.
		{
			auto stopView = world.GetView<AudioSource2DComponent, StopAudioSourceComponent>();
			for (auto entity: stopView)
			{
				auto &audioSource = stopView.get<AudioSource2DComponent>(entity);
				audioSource.Source.Stop();

				auto wrappedEntity = world.WrapIdentifierIntoEntity(entity);
				wrappedEntity.RemoveComponent<StopAudioSourceComponent>();
			}
		}
	}

	void PlayAudioSourceOnInitializeSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		auto view = world.GetView<AudioSource2DComponent, PlayAudioSourceOnInitializeComponent>();
		for (auto entity: view)
		{
			auto wrappedEntity = world.WrapIdentifierIntoEntity(entity);
			wrappedEntity.AddComponent<StartAudioSourceComponent>();
		}
	}
}