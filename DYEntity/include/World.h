#pragma once

#include <entt/entt.hpp>

namespace DYE::DYEntity
{
	class Entity;
	class World;

	class World
	{
		friend Entity;
	public:
		World();
		~World() = default;

		Entity CreateEntity();
		Entity CreateEntity(std::string const& name);

	private:
		entt::registry m_Registry;

	};
}