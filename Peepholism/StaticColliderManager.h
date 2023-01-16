#pragma once

#include "Math/AABB.h"
#include "Math/PrimitiveTest.h"

#include <optional>
#include <vector>

namespace DYE
{
	using ColliderID = std::int32_t;

	struct RaycastHit2D
	{
		ColliderID ColliderID;
		float Time;
		glm::vec2 Point;
	};

	class StaticColliderManager
	{
	public:
		ColliderID RegisterAABB(Math::AABB aabb);
		void UnregisterAABB(ColliderID id);

		std::optional<Math::AABB> GetAABB(ColliderID id);
		bool SetAABB(ColliderID id, Math::AABB aabb);

		std::vector<ColliderID> OverlapAABB(Math::AABB aabb) const;
		std::vector<ColliderID> OverlapCircle(glm::vec2 center, float radius) const;
		std::vector<RaycastHit2D> RaycastAll(glm::vec2 start, glm::vec2 end) const;
		std::vector<RaycastHit2D> CircleCastAll(glm::vec2 center, float radius, glm::vec2 direction) const;

		void DrawGizmos() const;
		void DrawImGui();

	private:
		int binarySearchIndexOf(std::vector<std::pair<ColliderID, Math::AABB>> const& collection, ColliderID id) const;

	private:
		ColliderID m_AABBIdCounter = 0;
		std::vector<std::pair<ColliderID, Math::AABB>> m_AABBs;
	};
}