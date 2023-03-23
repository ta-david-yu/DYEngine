#pragma once

#include <entt/entt.hpp>

namespace DYE::DYEntity
{
	class Entity;

	class World
	{
		// We need this so Entity could access m_Registry for entt operation.
		friend Entity;
	public:
		World();
		~World() = default;

		Entity CreateEntity();
		Entity CreateEntity(std::string const& name);

		template<typename Func>
		void ForEachEntity(Func function)
		{
			m_Registry.each(
				[this, function](auto entityHandle)
				{
					Entity entity (*this, entityHandle);
					function(entity);
				});
		}

		inline bool IsEmpty() const { return m_Registry.empty(); }

	private:
		entt::registry m_Registry;

		// TODO: vector<Entity>?

	};
}