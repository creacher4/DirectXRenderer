#pragma once

#include "Component.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <functional>

/**
 * @class Registry
 * @brief Core of the ECS implementation that manages entities and and their respective components.
 *
 * The registry is responsible for:
 * - Creating and destroying entities
 * - Adding components to entities
 * - Retrieving components from entities
 * - Querying entities with specific components
 *
 * @note This class uses type erasure to store components of different types in the same container.
 */
class Registry
{
public:
    /**
     * @brief Creates a new entity
     * @return The ID of the newly created entity
     */
    EntityID CreateEntity()
    {
        return nextEntityId++;
    }

    /**
     * @brief Destroys an entity and all of its components
     * @param entity The ID of the entity to destroy
     */
    void DestroyEntity(EntityID entity)
    {
        for (auto &componentTypeMap : componentMaps)
        {
            if (componentTypeMap.second.count(entity) > 0)
            {
                componentTypeMap.second.erase(entity);
            }
        }
    }

    /**
     * @brief Adds a component to an entity
     * @tparam T The component type (must inherit from Component)
     * @tparam Args Parameter pack for the component constructor arguments
     * @param entity The ID of the entity to add the component to
     * @param args The arguments to pass to the component constructor
     * @return T* pointer to the newly created component
     */
    template <typename T, typename... Args>
    T *AddComponent(EntityID entity, Args &&...args)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto componentType = std::type_index(typeid(T));

        auto mapIt = componentMaps.find(componentType);
        if (mapIt == componentMaps.end())
        {
            componentMaps.emplace(componentType, std::unordered_map<EntityID, std::unique_ptr<Component>>());
            mapIt = componentMaps.find(componentType);
        }

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T *componentPtr = component.get();
        mapIt->second.emplace(entity, std::move(component));
        return componentPtr;
    }

    /**
     * @brief Retrieves a component from an entity
     * @tparam T The component type to retrieve
     * @param entity The ID of the entity to retrieve the component from
     * @return T* pointer to the component, or nullptr if component not found
     */
    template <typename T>
    T *GetComponent(EntityID entity)
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto componentType = std::type_index(typeid(T));
        if (componentMaps.find(componentType) == componentMaps.end())
            return nullptr;

        auto &typeMap = componentMaps[componentType];
        if (typeMap.find(entity) == typeMap.end())
            return nullptr;

        return static_cast<T *>(typeMap[entity].get());
    }

    /**
     * @brief Retrieves all entities that have a specific component type
     * @tparam T The component type to query for
     * @return std::vector<EntityID> List of entity IDs that have the specified component
     */
    template <typename T>
    std::vector<EntityID> GetEntitiesWith()
    {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        std::vector<EntityID> result;
        auto componentType = std::type_index(typeid(T));

        if (componentMaps.find(componentType) != componentMaps.end())
        {
            auto &typeMap = componentMaps[componentType];
            result.reserve(typeMap.size());

            for (const auto &pair : typeMap)
            {
                result.push_back(pair.first);
            }
        }

        return result;
    }

    /**
     * @brief Retrieves the total number of entities in the registry
     * @return size_t The number of unique entities
     */
    size_t GetEntityCount() const
    {
        std::unordered_set<EntityID> uniqueEntities;

        for (const auto &componentTypeMap : componentMaps)
        {
            for (const auto &entityComponentPair : componentTypeMap.second)
            {
                uniqueEntities.insert(entityComponentPair.first);
            }
        }

        return uniqueEntities.size();
    }

private:
    /** @brief ID to assign to the next entity created */
    EntityID nextEntityId = 1;

    /**
     * @brief Main storage for all components
     *
     * Organized as:
     * - Outer map: Component type → map of entities with this component
     * - Inner map: Entity ID → Component instance
     */
    std::unordered_map<std::type_index, std::unordered_map<EntityID, std::unique_ptr<Component>>> componentMaps;
};