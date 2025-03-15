#pragma once

#include <cstdint>

using EntityID = std::uint32_t;
constexpr EntityID INVALID_ENTITY = 0;

class Component
{
public:
    virtual ~Component() = default;
};