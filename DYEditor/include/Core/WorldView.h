#pragma once

#include <entt/entt.hpp>

namespace DYE::DYEditor
{
	template<typename... Type>
	using TypeList = entt::type_list<Type...>;

	template<typename... Type>
	using Exclude = TypeList<Type...>;

	template<typename... Type>
	using Get = TypeList<Type...>;

	template<typename, typename, typename = void>
	class WorldView;

	template<typename... Components, typename... Excludes>
	class WorldView<Get<Components...>, Exclude<Excludes...>> : public entt::basic_view<entt::get_t<Components...>, entt::exclude_t<Excludes...>>
	{
	};

/*
	template<typename, typename, typename, typename = void>
	class WorldView;

	template<typename Entity, typename... Component, typename... Exclude>
	class WorldView<Entity, GetType<Component...>, ExcludeType<Exclude...>>
	{
	public:
		entt::basic_view <Entity, GetType<Component...>, ExcludeType<Exclude...>> View;
	};*/
}