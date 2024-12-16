#include "physics.h"

#include "application.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void Physics::applyVelocity(Object* obj, const float(&direction)[3], float speed)
{
    glm::vec3 dir = glm::normalize(glm::vec3(direction[0], direction[1], direction[2]));
    float velSpeed = speed * Application::get().deltaTime;

    obj->mPosition[0] += dir.x * velSpeed;
    obj->mPosition[1] += dir.y * velSpeed;
    obj->mPosition[2] += dir.z * velSpeed;
}
