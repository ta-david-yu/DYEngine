#include "Systems/TransformSystems.h"

#include "Core/Entity.h"
#include "Components/TransformComponents.h"
#include "ImGui/ImGuiUtil.h"

#include <algorithm>
#include <execution>

namespace DYE::DYEditor
{

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

		auto group = world.GetRegistry().group<LocalToWorldComponent>(Get<LocalTransformComponent>, Exclude<ParentComponent, ChildrenComponent>);
		std::for_each
		(
			std::execution::par_unseq, group.begin(), group.end(),
			[&group, &world](auto entityIdentifier)
			{
				LocalToWorldComponent &localToWorld = group.get<LocalToWorldComponent>(entityIdentifier);
				LocalTransformComponent localTransformComponent = group.get<LocalTransformComponent>(entityIdentifier);
				localToWorld.Matrix = localTransformComponent.GetTransformMatrix();
			}
		);
	}

	void ComputeLocalToWorldSystem::DrawInspector(World &world)
	{
		auto group = world.GetRegistry().group<LocalToWorldComponent>(Get<LocalTransformComponent>, Exclude<ParentComponent, ChildrenComponent>);
		std::for_each
		(
			group.begin(), group.end(),
			[&group, &world](auto entityIdentifier)
			{
				auto [localToWorld, localTransform] = group.get<LocalToWorldComponent, LocalTransformComponent>(entityIdentifier);

				Entity entity = world.WrapIdentifierIntoEntity(entityIdentifier);
				//DYE_LOG("%s", entity.GetName().c_str());
				ImGui::TextUnformatted(entity.GetName().c_str());
			}
		);
	}
}