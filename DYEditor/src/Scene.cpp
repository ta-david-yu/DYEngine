#include "Scene.h"

#include "Util/Macro.h"

namespace DYE::DYEditor
{

	std::vector<SystemDescriptor>& Scene::GetSystemDescriptorsOfPhase(ExecutionPhase phase)
	{
		switch (phase)
		{
			case ExecutionPhase::Initialize:
				return InitializeSystemDescriptors;
			case ExecutionPhase::FixedUpdate:
				return FixedUpdateSystemDescriptors;
			case ExecutionPhase::Update:
				return UpdateSystemDescriptors;
			case ExecutionPhase::LateUpdate:
				return LateUpdateSystemDescriptors;
			case ExecutionPhase::Render:
				return RenderSystemDescriptors;
			case ExecutionPhase::ImGui:
				return ImGuiSystemDescriptors;
			case ExecutionPhase::Cleanup:
				return CleanupSystemDescriptors;
			case ExecutionPhase::TearDown:
				return TearDownSystemDescriptors;
			default:
				DYE_ASSERT(false && "Unknown execution phase, return InitializeSystemTypeNames.");
				return UnrecognizedSystems;
		}
	}

	GroupID Scene::AddOrGetGroupID(const std::string_view &groupName)
	{
		for (GroupID id = 0; id < SystemGroupNames.size(); ++id)
		{
			if (SystemGroupNames[id] == groupName)
			{
				return id;
			}
		}

		SystemGroupNames.emplace_back(groupName);
		return SystemGroupNames.size() - 1;
	}
}

