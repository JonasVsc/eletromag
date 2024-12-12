#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera();

	glm::mat4 getViewMatrix();

	glm::vec3 mPosition;
	glm::vec3 mFront;
	glm::vec3 mUp;
};