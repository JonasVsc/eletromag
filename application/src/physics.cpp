#include "physics.h"

#include "application.h"



void Physics::applyVelocity(Object* obj, const float(&direction)[3], float speed)
{
    if(direction[0] == 0 && direction[1] == 0 && direction[2] == 0)
        return;
    glm::vec3 dir = glm::normalize(glm::vec3(direction[0], direction[1], direction[2]));
    float velSpeed = speed * Application::get().deltaTime;

    obj->mPosition[0] += dir.x * velSpeed;
    obj->mPosition[1] += dir.y * velSpeed;
    obj->mPosition[2] += dir.z * velSpeed;
}

void Physics::applyVelocityByCurrentDirection(Object* obj, float speed)
{

}

void Physics::crossVectorialProduct(float(&res)[3], const float(&vec1)[3], const float(&vec2)[3])
{
    res[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
    res[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
    res[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
}