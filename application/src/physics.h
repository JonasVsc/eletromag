#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Object;

class Physics
{
public:

    static void applyVelocity(Object* obj, const float(&direction)[3], float speed);
    static void applyVelocityByCurrentDirection(Object* obj, float speed); // for objects which act like vectors

    static void crossVectorialProduct(float(&res)[3], const float(&vec1)[3], const float(&vec2)[3]);
    static void rotateToTarget(glm::mat4 &modelMatrix, const float(&currentPosition)[3], const float(&targetPosition)[3]);



private:

    Physics();

};