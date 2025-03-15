#pragma once

#include "Registry.h"

class System
{
public:
    System(Registry &registry) : registry(registry) {}
    virtual ~System() = default;

    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) = 0;

protected:
    Registry &registry;
};