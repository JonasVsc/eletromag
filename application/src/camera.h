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

	float fov = 45.0f; 
	float aspectRatio = 1920.0f / 1080.0f; 
	float nearPlane = 0.1f; 
	float farPlane = 1000.0f; 

	bool firstMouse = true;
    bool rightbuttonPressed = false;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 640.0f / 2.0;
	float lastY = 480.0 / 2.0;

};