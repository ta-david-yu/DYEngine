#pragma once

#include "Util/Macro.h"
#include "Core/World.h"
#include "Components/HierarchyComponents.h"

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <stack>

#include <entt/entt.hpp>

namespace DYE::DYEditor
{
    template<typename T>
    /// A concept that checks if the given type doesn't have any member.
    concept IsEmptyStruct = std::is_empty_v<T>;

    /// We want to have a light-weight wrapper around the underlying implementation.
    /// The wrapper Entity class should be trivially-copyable.
    class Entity
    {
        // We need this so World could call the private ctor.
        friend class World;

    public:
        static Entity Null();

        /// Create a null entity.
        Entity() = default;
        Entity(Entity const &other) = default;

        World &GetWorld() { return *m_pWorld; }
        EntityIdentifier GetIdentifier() const { return m_EntityIdentifier; }
        EntityInstanceID GetInstanceID() const { return static_cast<EntityInstanceID>(m_EntityIdentifier); }
        bool IsValid() const;

        template<typename T, typename... Args>
        T &AddComponent(Args &&...args)
        {
            DYE_ASSERT(!this->HasComponent<T>() && "Entity::PushSerializedComponent: Entity already has component of the given type.");
            return m_pWorld->m_Registry.emplace<T>(m_EntityIdentifier, std::forward<Args>(args)...);
        }

        /// AddComponent for empty component (i.e. struct with no members)
        template<typename T>
        requires IsEmptyStruct<T>
        void AddComponent()
        {
            DYE_ASSERT(!this->HasComponent<T>() && "Entity::PushSerializedComponent: Entity already has component of the given type.");
            m_pWorld->m_Registry.emplace<T>(m_EntityIdentifier);
        }

        template<typename T, typename... Args>
        T &AddOrGetComponent(Args &&...args)
        {
            if (this->HasComponent<T>())
            {
                return GetComponent<T>();
            }
            return m_pWorld->m_Registry.emplace<T>(m_EntityIdentifier, std::forward<Args>(args)...);
        }

        /// AddOrGetComponent for empty component (i.e. struct with no members)
        template<typename T>
        requires IsEmptyStruct<T>
        void AddOrGetComponent()
        {
            if (this->HasComponent<T>())
            {
                return;
            }
            m_pWorld->m_Registry.emplace<T>(m_EntityIdentifier);
        }

        /// \return the number of removed components. 0 if the entity doesn't own the given component.
        template<typename T>
        std::size_t RemoveComponent()
        {
            return m_pWorld->m_Registry.remove<T>(m_EntityIdentifier);
        }

        template<typename T>
        T &GetComponent()
        {
            DYE_ASSERT(this->HasComponent<T>() && "Entity::GetComponent: Entity doesn't have component of the given type.");
            return m_pWorld->m_Registry.get<T>(m_EntityIdentifier);
        }

        /// Syntactic sugar for TryGetComponent<NameComponent>().Name
        /// \return
        std::optional<std::string> TryGetName();
        std::string GetName();

        std::optional<GUID> TryGetGUID();

        template<typename T>
        std::optional<std::reference_wrapper<T>> TryGetComponent()
        {
            if (!this->HasComponent<T>())
            {
                return {};
            }
            return m_pWorld->m_Registry.get<T>(m_EntityIdentifier);
        }

        template<typename T>
        bool HasComponent() const
        {
            return m_pWorld->m_Registry.all_of<T>(m_EntityIdentifier);
        }

        void RemoveAllComponents();

        bool operator==(Entity const &other) const
        {
            return m_EntityIdentifier == other.m_EntityIdentifier && m_pWorld == other.m_pWorld;
        }

        bool operator!=(Entity const &other) const
        {
            return !(*this == other);
        }

    private:
        /// Create an Entity with the given World & the internal handle.
        explicit Entity(World &world, EntityIdentifier identifier);

        World *m_pWorld = nullptr;
        EntityIdentifier m_EntityIdentifier = entt::null;
    };
}