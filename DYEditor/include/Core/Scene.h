#pragma once

#include "Core/EditorSystem.h"
#include "World.h"

#include <vector>
#include <string>
#include <unordered_map>

namespace DYE::DYEditor
{
	using GroupID = std::int32_t;

	constexpr GroupID NoSystemGroupID = -1;

	struct SystemDescriptor
	{
		std::string Name;
		GroupID Group = NoSystemGroupID;
		bool IsEnabled = true;
		
		SystemBase* Instance = nullptr;
	};

	class Scene
	{
		friend class SerializedObjectFatory;

	public:
		Scene() = default;

		std::vector<SystemDescriptor>& GetSystemDescriptorsOfPhase(ExecutionPhase phase);

		/// Foreach for all system descriptor from different event phases, this does not include systems which are unrecognized.
		template<typename Func>
		void ForEachSystemDescriptor(Func function) const
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
			for (const auto& systemDescriptor : PostRenderSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::PostRender);
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

		/// Foreach for all system descriptor from different event phases, this does not include systems which are unrecognized.
		template<typename Func>
		void ForEachSystemDescriptor(Func function)
		{
			for (auto& systemDescriptor : InitializeSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Initialize);
			}
			for (auto& systemDescriptor : FixedUpdateSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::FixedUpdate);
			}
			for (auto& systemDescriptor : UpdateSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Update);
			}
			for (auto& systemDescriptor : LateUpdateSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::LateUpdate);
			}
			for (auto& systemDescriptor : RenderSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Render);
			}
			for (auto& systemDescriptor : PostRenderSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::PostRender);
			}
			for (auto& systemDescriptor : ImGuiSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::ImGui);
			}
			for (auto& systemDescriptor : CleanupSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::Cleanup);
			}
			for (auto& systemDescriptor : TearDownSystemDescriptors)
			{
				function(systemDescriptor, ExecutionPhase::TearDown);
			}
		}

		GroupID AddOrGetGroupID(std::string_view const& groupName);

		/// If a system with the given name cannot be found in the TypeRegistry, returned systemDescriptor.Instance will be nullptr.
		SystemDescriptor TryAddSystemByName(std::string const &systemName);

		/// Remove all entities and systems. Basically make the scene empty.
		void Clear();

		bool IsEmpty() const;

		/// Execute initialize systems in the active scene.
		void ExecuteInitializeSystems();
		void ExecuteTeardownSystems();

	public:
		std::string Name;
		// At runtime, we will use the names to reference the actual systems from TypeRegistry.
		// For different phase types, see ExecutionPhase of SystemBase.
		std::vector<SystemDescriptor> InitializeSystemDescriptors;
		std::vector<SystemDescriptor> FixedUpdateSystemDescriptors;
		std::vector<SystemDescriptor> UpdateSystemDescriptors;
		std::vector<SystemDescriptor> LateUpdateSystemDescriptors;
		std::vector<SystemDescriptor> RenderSystemDescriptors;
		std::vector<SystemDescriptor> PostRenderSystemDescriptors;
		std::vector<SystemDescriptor> ImGuiSystemDescriptors;
		std::vector<SystemDescriptor> CleanupSystemDescriptors;
		std::vector<SystemDescriptor> TearDownSystemDescriptors;
		std::vector<SystemDescriptor> UnrecognizedSystems;
		DYEditor::World World;

		/// We use the index as GroupID.
		std::vector<std::string> SystemGroupNames;
	};
}