#pragma once

#include "World.h"
#include <entt/entt.hpp>

namespace DYE::DYEntity
{
	// TODO:

	template<typename, typename, typename, typename = void>
	class WorldView;

	template<typename Entity, typename... Component, typename... Exclude>
	class WorldView<Entity, entt::get_t<Component...>, entt::exclude_t<Exclude...>> : protected entt::basic_view<Entity, entt::get_t<Component...>, entt::exclude_t<Exclude...>>
	{};
}