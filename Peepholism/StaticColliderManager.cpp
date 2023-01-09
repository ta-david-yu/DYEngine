#include "StaticColliderManager.h"

#include "ImGui/ImGuiUtil.h"
#include "Graphics/DebugDraw.h"
#include "Math/Color.h"

#include <imgui.h>
#include <string>

namespace DYE
{

	ColliderID StaticColliderManager::RegisterAABB(Math::AABB aabb)
	{
		ColliderID const id = m_AABBIdCounter;
		m_AABBs.emplace_back(id, aabb);
		m_AABBIdCounter++;
		return id;
	}

	void StaticColliderManager::UnregisterAABB(ColliderID id)
	{
		int index = binarySearchIndexOf(m_AABBs, id);
		if (index == -1)
		{
			return;
		}

		m_AABBs.erase(m_AABBs.begin() + index);
	}

	std::optional<Math::AABB> StaticColliderManager::GetAABB(ColliderID id)
	{
		int const index = binarySearchIndexOf(m_AABBs, id);
		if (index == -1)
		{
			return {};
		}

		return m_AABBs[index].second;
	}

	bool StaticColliderManager::SetAABB(ColliderID id, Math::AABB aabb)
	{
		int const index = binarySearchIndexOf(m_AABBs, id);
		if (index == -1)
		{
			return false;
		}

		m_AABBs[index].second = aabb;
		return true;
	}

	void StaticColliderManager::DrawGizmos() const
	{
		for (auto const& pair : m_AABBs)
		{
			DebugDraw::AABB(pair.second.Min, pair.second.Max, Color::Blue);
		}
	}

	void StaticColliderManager::DrawImGui()
	{
		if (ImGui::Begin("Collider Manager"))
		{
			for (auto& aabbPair : m_AABBs)
			{
				std::string const label = "AABB " + std::to_string(aabbPair.first);
				ImGuiUtil::DrawAABBControl(label, aabbPair.second);
			}
		}

		ImGui::End();
	}

	int StaticColliderManager::binarySearchIndexOf(std::vector<std::pair<ColliderID, Math::AABB>> const& collection, ColliderID id) const
	{
		int left = 0;
		int right = m_AABBs.size() - 1;
		while (left <= right)
		{
			int const mid = left + (right - left) / 2;
			ColliderID const midId = m_AABBs[mid].first;

			if (midId == id)
			{
				return mid;
			}

			if (midId < id)
			{
				left = mid + 1;
				continue;
			}

			if (midId > id)
			{
				right = mid - 1;
				continue;
			}
		}

		return -1;
	}
}