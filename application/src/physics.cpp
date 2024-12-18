#include "physics.h"

#include "application.h"



void Physics::applyVelocity(Object* obj, glm::vec3 direction, float speed)
{
    if(direction.x == 0 && direction.y == 0 && direction.z == 0)
        return;

    glm::vec3 dir = glm::normalize(direction);
    float velSpeed = speed * Application::get().deltaTime;

    // obj->mPosition[0] += dir.x * velSpeed;
    // obj->mPosition[1] += dir.y * velSpeed;
    // obj->mPosition[2] += dir.z * velSpeed;

    obj->transform.mPosition += dir * velSpeed;
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