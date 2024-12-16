#pragma once

class Object;

class Physics
{
public:

    static void applyVelocity(Object* obj, const float(&direction)[3], float speed);

private:

    Physics();

};