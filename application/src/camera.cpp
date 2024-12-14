#include "camera.h"


Camera::Camera()
	:mPosition(glm::vec3(0.0f, 5.0f, 50.0f)), mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mUp(glm::vec3(0.0f, 1.0f, 0.0f))
{
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(mPosition, mPosition + mFront, mUp);
}