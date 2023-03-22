#pragma once

#include <entt/entt.hpp>

namespace DYE::DYEntity
{
	class Entity;

	//using ForEachEntityFunction = void (Entity& entity);
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

	private:
		entt::registry m_Registry;

	};
}