#pragma once

#include "Math/AABB.h"
#include "Math/PrimitiveTest.h"

#include <optional>
#include <vector>

namespace DYE
{
	using ColliderID = std::int32_t;

	class StaticColliderManager
	{
	public:
		ColliderID RegisterAABB(Math::AABB aabb);
		void UnregisterAABB(ColliderID id);

		std::optional<Math::AABB> GetAABB(ColliderID id);
		bool SetAABB(ColliderID id, Math::AABB aabb);

		std::vector<ColliderID> OverlapAABB(Math::AABB aabb) const;
		std::vector<ColliderID> OverlapCircle(glm::vec3 center, float radius) const;

		void DrawGizmos() const;
		void DrawImGui();

	private:
		int binarySearchIndexOf(std::vector<std::pair<ColliderID, Math::AABB>> const& collection, ColliderID id) const;

	private:
		ColliderID m_AABBIdCounter = 0;
		std::vector<std::pair<ColliderID, Math::AABB>> m_AABBs;
	};
}