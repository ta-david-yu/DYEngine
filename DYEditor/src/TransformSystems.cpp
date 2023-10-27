#include "Systems/TransformSystems.h"

#include "Core/Entity.h"
#include "Components/TransformComponents.h"
#include "ImGui/ImGuiUtil.h"

#include <algorithm>
#include <execution>

namespace DYE::DYEditor
{
	void ComputeLocalToWorldSystem::InitializeLoad(World &world, DYE::DYEditor::InitializeLoadParameters)
	{
		auto syncGroup = world.GetRegistry().group<LocalToWorldComponent, LocalTransformComponent>({}, Exclude<ParentComponent>);
	}
	
	void ComputeLocalToWorldSystem::Execute(World &world, DYE::DYEditor::ExecuteParameters params)
	{
		// TODO: ComputeLocalToWorldSystem Options:
		// 1. Use observer to iterate through entity with changed transform components in the last tick, update LocalToWorld system.
		// 	  This requires manually calling registry.patch or register that entity in the list.
		// 2. Having an 'IsChanged' flag in transform component, add function interface (getter/setter) to read/write transform data &
		//	  set 'IsChanged' to true. Then we can iterate through all transform, but only ones with 'IsChanged' equal to true.
		//	  This option is self-contained, and potentially better than (4) if we are going for the brute force method.
		// 3. Combined (1) & (2), having an 'IsTransformDirty' component and add it to the entity on transform data changed. This
		//    requires passing entity in the set transform data functions.
		// 4. Don't care, always go through all the transforms in the hierarchy.
		//
		// We can further use std::for_each and std::execution::par to speed up the process.
		// Right now we might go with (2) & (4) combined + parallel operation.
		// see https://skypjack.github.io/entt/md_docs_md_entity.html, Multithreading/Iterators section


		// Synchronize LocalToWorld for root transforms.
		auto syncGroup = world.GetRegistry().group<LocalToWorldComponent, LocalTransformComponent>({}, Exclude<ParentComponent>);
		std::for_each
		(
			std::execution::par_unseq, syncGroup.begin(), syncGroup.end(),
			[&syncGroup](auto entityIdentifier)
			{
				LocalToWorldComponent &localToWorld = syncGroup.get<LocalToWorldComponent>(entityIdentifier);
				LocalTransformComponent localTransformComponent = syncGroup.get<LocalTransformComponent>(entityIdentifier);

				localToWorld.Matrix = localTransformComponent.GetTransformMatrix();
			}
		);

		// Compute & propagate LocalToWorld from root transforms down to their children recursively.
		auto propagationView = world.GetRegistry().view<LocalToWorldComponent, ChildrenComponent>(Exclude<ParentComponent>);
		std::for_each
		(
			std::execution::par_unseq, propagationView.begin(), propagationView.end(),
			[&propagationView, &world](auto entityIdentifier)
			{
				LocalToWorldComponent &rootParentLocalToWorld = propagationView.get<LocalToWorldComponent>(entityIdentifier);
				ChildrenComponent &childrenComponent = propagationView.get<ChildrenComponent>(entityIdentifier);

				std::vector<EntityIdentifier> const& childrenEntityIdentifiers = childrenComponent.GetChildrenCache();
				std::for_each
				(
					std::execution::par_unseq, childrenEntityIdentifiers.begin(), childrenEntityIdentifiers.end(),
					[&world, rootParentLocalToWorld](auto childEntityIdentifier)
					{
						computeLocalToWorldRecursively(world, rootParentLocalToWorld.Matrix, childEntityIdentifier);
					}
				);
			}
		);
	}

	void computeLocalToWorldRecursively(World &world, glm::mat4 parentToWorld, EntityIdentifier entityIdentifier)
	{
		Entity entity = world.WrapIdentifierIntoEntity(entityIdentifier);

		DYE_ASSERT_LOG_WARN(entityIdentifier != entt::null, "The cached entity identifier in children component is invalid.");

		glm::mat4 localToParent = glm::mat4 {1.0f};	// By default, local to parent transformation matrix is an identity matrix, meaning there is no transform offset to the parent.
		auto tryGetLocalTransform = entity.TryGetComponent<LocalTransformComponent>();
		if (tryGetLocalTransform.has_value())
		{
			localToParent = tryGetLocalTransform.value().get().GetTransformMatrix();
		}

		glm::mat4 localToWorld = parentToWorld * localToParent;
		auto tryGetLocalToWorld = entity.TryGetComponent<LocalToWorldComponent>();
		if (tryGetLocalToWorld.has_value())
		{
			tryGetLocalToWorld.value().get().Matrix = localToWorld;
		}

		// Propagate local to world to the children if there is any.
		auto tryGetChildrenComponent = entity.TryGetComponent<ChildrenComponent>();
		if (!tryGetChildrenComponent.has_value())
		{
			return;
		}

		std::vector<EntityIdentifier> const& childrenEntityIdentifiers = tryGetChildrenComponent.value().get().GetChildrenCache();
		std::for_each
		(
			std::execution::par_unseq, childrenEntityIdentifiers.begin(), childrenEntityIdentifiers.end(),
			[&world, localToWorld](auto childEntityIdentifier)
			{
				computeLocalToWorldRecursively(world, localToWorld, childEntityIdentifier);
			}
		);
	}

	void ComputeLocalToWorldSystem::DrawInspector(World &world)
	{
		ImGui::TextUnformatted("Sync Group");
		ImGui::Separator();
		auto syncGroup = world.GetRegistry().group<LocalToWorldComponent, LocalTransformComponent>({}, Exclude<ParentComponent>);
		std::for_each
		(
			syncGroup.begin(), syncGroup.end(),
			[&world](auto entityIdentifier)
			{
				Entity entity = world.WrapIdentifierIntoEntity(entityIdentifier);
				ImGui::TextUnformatted(entity.GetName().c_str());
			}
		);

		ImGui::TextUnformatted("Propagation Group");
		ImGui::Separator();
		auto propagationView = world.GetRegistry().view<LocalToWorldComponent, ChildrenComponent>(Exclude<ParentComponent>);
		std::for_each
		(
			propagationView.begin(), propagationView.end(),
			[&world](auto entityIdentifier)
			{
				Entity entity = world.WrapIdentifierIntoEntity(entityIdentifier);
				ImGui::TextUnformatted(entity.GetName().c_str());
			}
		);
	}
}