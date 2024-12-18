#pragma once



class Transform
{
public:
    glm::vec3 mPosition = glm::vec3(0.0f);
    glm::vec3 mRotation = glm::vec3(0.0f);
    glm::vec3 mScale = glm::vec3(1.0f);

    glm::vec3 mInitialPosition = mPosition;
    glm::vec3 mInitialRotation = mRotation;
    glm::vec3 mInitialScale = mScale;
};