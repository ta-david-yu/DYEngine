#pragma once

#include <entt/entt.hpp>

namespace DYE::DYEntity
{
	class Entity
	{
	public:
		Entity() = default;

	private:
		entt::entity m_EntityHandle;
	};
}