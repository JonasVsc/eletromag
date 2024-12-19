#pragma once

#include "object.h"

class Object;

class Component {
public:
    Object* parent;

    virtual void update(float deltaTime) = 0;
    virtual ~Component() = default;
};