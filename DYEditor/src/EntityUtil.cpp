#include "Util/EntityUtil.h"

#include "Components/IDComponent.h"

namespace DYE::DYEditor::EntityUtil
{
	bool IsFirstDegreeChildOf(Entity potentialChild, Entity potentialParent)
	{
		if (!potentialChild.HasComponent<ParentComponent>())
		{
			return false;
		}

		if (!potentialParent.HasComponent<ChildrenComponent>())
		{
			return false;
		}

		if (potentialChild == potentialParent)
		{
			return false;
		}

		return potentialChild.GetComponent<ParentComponent>().ParentGUID == potentialParent.GetComponent<IDComponent>().ID;
	}

	bool IsChildOf(Entity potentialChild, Entity potentialParent)
	{
		if (!potentialChild.HasComponent<ParentComponent>())
		{
			return false;
		}

		if (!potentialParent.HasComponent<ChildrenComponent>())
		{
			return false;
		}

		if (potentialChild == potentialParent)
		{
			return false;
		}

		// Go through all children using DFS.
		std::stack<Entity> entityStack;
		entityStack.push(potentialParent);
		bool isRoot = true;
		while (!entityStack.empty())
		{
			Entity entity = entityStack.top();

			// We use a flag to skip the first entity in the stack (which is the parent).
			if (!isRoot)
			{
				if (entity == potentialChild)
				{
					// We find a child in the potential parent!
					return true;
				}
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
				auto tryGetEntityWithGUID = potentialParent.GetWorld().TryGetEntityWithGUID(childGUID);
				if (!tryGetEntityWithGUID.has_value())
				{
					continue;
				}
				// Push the child into the stack.
				entityStack.push(tryGetEntityWithGUID.value());
			}
		}

		return false;
	}

	std::vector<Entity> GetEntityAndAllChildrenPreorder(Entity root)
	{
		std::vector<Entity> result;

		std::stack<Entity> entityStack;
		entityStack.push(root);

		while (!entityStack.empty())
		{
			Entity entity = entityStack.top();

			result.push_back(entity);

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

		return std::move(result);
	}

	std::vector<Entity> GetAllChildrenPreorder(Entity root)
	{
		std::vector<Entity> result;

		std::stack<Entity> entityStack;
		entityStack.push(root);

		bool isRoot = true;

		while (!entityStack.empty())
		{
			Entity entity = entityStack.top();

			// We use a flag to skip the first entity in the stack (which is the parent).
			if (!isRoot)
			{
				result.push_back(entity);
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

		return std::move(result);
	}
}