#include "Core/Entity.h"

namespace DYE::DYEditor::EntityUtil
{
	/// Iterate through the entity and its all children (including nth degree children)
	/// using depth-first search.
	template<typename Func>
	void ForEntityAndEachChildRecursive(Entity root, Func func)
	{
		std::stack<Entity> entityStack;
		entityStack.push(root);

		while (!entityStack.empty())
		{
			Entity entity = entityStack.top();

			func(entity);

			entityStack.pop();
			auto tryGetChild = entity.TryGetComponent<ChildrenComponent>();
			if (!tryGetChild.has_value())
			{
				// No child, nothing to push into the stack.
				continue;
			}

			auto &childrenGUIDs = tryGetChild.value().get().ChildrenGUIDs;
			for (int i = childrenGUIDs.size() - 1; i >= 0; i--)
			{
				auto childGUID = childrenGUIDs[i];
				auto tryGetEntityWithGUID = root.GetWorld().TryGetEntityWithGUID(childGUID);
				if (!tryGetEntityWithGUID.has_value())
				{
					continue;
				}
				// Push the child into the stack.
				entityStack.push(tryGetEntityWithGUID.value());
			}
		}
	}

	/// Iterate through all children (including nth degree children)
	/// using depth-first search.
	template<typename Func>
	void ForEachChildRecursive(Entity root, Func func)
	{
		std::stack<Entity> entityStack;
		entityStack.push(root);

		bool isRoot = true;

		while (!entityStack.empty())
		{
			Entity entity = entityStack.top();

			// We use a flag to skip the first entity in the stack (which is the parent).
			if (!isRoot)
			{
				func(entity);
			}
			isRoot = false;

			entityStack.pop();
			auto tryGetChild = entity.TryGetComponent<ChildrenComponent>();
			if (!tryGetChild.has_value())
			{
				// No child, nothing to push into the stack.
				continue;
			}

			auto &childrenGUIDs = tryGetChild.value().get().ChildrenGUIDs;
			for (int i = childrenGUIDs.size() - 1; i >= 0; i--)
			{
				auto childGUID = childrenGUIDs[i];
				auto tryGetEntityWithGUID = root.GetWorld().TryGetEntityWithGUID(childGUID);
				if (!tryGetEntityWithGUID.has_value())
				{
					continue;
				}
				// Push the child into the stack.
				entityStack.push(tryGetEntityWithGUID.value());
			}
		}
	}
}