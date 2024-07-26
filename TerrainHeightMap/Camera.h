#pragma once

#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(GLFWwindow* window) : window(window) {}
	~Camera() {};

	void processKeyInput();
	void mouseControl(GLfloat xChange, GLfloat yChange);

	glm::mat4 calculateViewMatrix();

private:	
	GLFWwindow* window;
	
	glm::vec3 cameraPos		= glm::vec3(0.f, 0.f,  3.f);
	glm::vec3 cameraFront	= glm::vec3(0.f, 0.f, -1.f);
	glm::vec3 cameraUp		= glm::vec3(0.f, 1.f,  0.f);
	glm::vec3 worldUp		= glm::vec3(0.f, 1.f,  0.f);

	float yaw = -90.f, pitch = 0.f;
	const float lookSensitivity = 0.1f;

	float deltaTime = 0.f;
	float lastTime = 0.f;
	
	void update();
};

void Camera::processKeyInput()
{
	float currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	const float cameraSpeed = 4.f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos -= cameraSpeed * worldUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraSpeed * worldUp;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void Camera::mouseControl(GLfloat xOffset, GLfloat yOffset)
{		
	xOffset *= lookSensitivity;
	yOffset *= lookSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	update();
}

void Camera::update()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}
