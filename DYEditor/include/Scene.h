#pragma once

#include "EditorSystem.h"
#include "World.h"

#include <vector>
#include <string>
#include <unordered_map>

#define NO_SYSTEM_GROUP_ID -1

namespace DYE::DYEditor
{
	using GroupID = std::int32_t;

	struct SystemDescriptor
	{
		std::string Name;
		GroupID Group = NO_SYSTEM_GROUP_ID;
	};

	class Scene
	{
		friend class SerializedObjectFatory;

	public:
		Scene() = default;

		std::vector<SystemDescriptor>& GetSystemDescriptorsOfPhase(ExecutionPhase phase);

		template<typename Func>
		void ForEachSystemTypeName(Func function)
		{
			for (const auto& systemDescriptor : InitializeSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Initialize);
			}
			for (const auto& systemDescriptor : FixedUpdateSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::FixedUpdate);
			}
			for (const auto& systemDescriptor : UpdateSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Update);
			}
			for (const auto& systemDescriptor : LateUpdateSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::LateUpdate);
			}
			for (const auto& systemDescriptor : RenderSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Render);
			}
			for (const auto& systemDescriptor : ImGuiSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::ImGui);
			}
			for (const auto& systemDescriptor : CleanupSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Cleanup);
			}
			for (const auto& systemDescriptor : TearDownSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::TearDown);
			}
		}

		GroupID AddOrGetGroupID(std::string_view const& groupName);

	public:
		std::string Name;
		// At runtime, we will use the names to reference the actual systems from TypeRegistry.
		// For different phase types, see ExecutionPhase of SystemBase.
		std::vector<SystemDescriptor> InitializeSystemDescriptors;
		std::vector<SystemDescriptor> FixedUpdateSystemDescriptors;
		std::vector<SystemDescriptor> UpdateSystemDescriptors;
		std::vector<SystemDescriptor> LateUpdateSystemDescriptors;
		std::vector<SystemDescriptor> RenderSystemDescriptors;
		std::vector<SystemDescriptor> ImGuiSystemDescriptors;
		std::vector<SystemDescriptor> CleanupSystemDescriptors;
		std::vector<SystemDescriptor> TearDownSystemDescriptors;
		std::vector<SystemDescriptor> UnrecognizedSystems;
		DYEntity::World World;

		/// We use the index as GroupID.
		std::vector<std::string> SystemGroupNames;
	};
}