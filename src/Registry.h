#pragma once

#include "Component.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <functional>

// needs a LOT of documentation
class Registry
{
public:
    EntityID CreateEntity()
    {
        return nextEntityId++;
    }

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
    EntityID nextEntityId = 1;
    std::unordered_map<std::type_index, std::unordered_map<EntityID, std::unique_ptr<Component>>> componentMaps;
};
