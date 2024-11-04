#pragma once

#include <entt/entt.hpp>

namespace DYE::DYEditor
{
    template<typename... Type>
    using TypeList = entt::type_list<Type...>;

    template<typename... Type>
    using Exclude_t = entt::exclude_t<Type...>;

    template<typename... Type>
    inline constexpr Exclude_t<Type...> Exclude {};

    template<typename... Type>
    using Get_t = entt::get_t<Type...>;

    template<typename... Type>
    inline constexpr Get_t<Type...> Get {};

    // The following view wrappers are abandoned for now.

    /*
    template<typename, typename, typename = void>
    class WorldView;

    template<typename... Components, typename... Excludes>
    class WorldView<Get_t<Components...>, Exclude_t<Excludes...>> : public entt::basic_view<entt::get_t<Components...>, entt::exclude_t<Excludes...>>
    {
    };


    template<typename, typename, typename, typename = void>
    class WorldView;

    template<typename Entity, typename... Components, typename... Excludes>
    class WorldView<Entity, Get_t<Components...>, Exclude_t<Excludes...>> : protected entt::basic_view<Entity, entt::get_t < Components...>, entt::exclude_t<Excludes...>>
    {
    public:
        entt::basic_view <Entity, Get_t<Components...>, Exclude_t<Excludes...>> View;
    };*/
}