#pragma once

#include "entt/entt.hpp"

namespace DYEngine::DYEntity
{
	class Entity
	{
	public:
		Entity() = default;

	private:
		entt::entity m_EntityHandle;
	};
}