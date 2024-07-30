#pragma once

#include <iostream>

#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"

#include "Camera.h"

class Window
{
public:
	Window(int width, int height, bool fullScreen);
	~Window();

	GLFWwindow* getWindow() const { return window; }
	int getWidth() const { return WIDTH; }
	int getHeight() const { return HEIGHT; }

	GLfloat getXChange();
	GLfloat getYChange();

private:
	GLFWwindow* window;
	const int WIDTH;
	const int HEIGHT;
	const bool fullscreen;

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMove;

	Camera* camera;
	
	void init();
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

Window::Window(int width, int height, bool fullScreen) : WIDTH(width), HEIGHT(height), fullscreen(fullScreen)
{
	xChange = 0.f;
	yChange = 0.f;
	mouseFirstMove = true;
	
	init();
}

Window::~Window()
{
	if (window)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}

void Window::init()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(
		WIDTH,
		HEIGHT,
		"TerrainHeightMapWindow",
		fullscreen ? glfwGetPrimaryMonitor() : nullptr,
		nullptr
	);

	if (window == nullptr)
	{
		std::cout << "Failed to create window!\n";
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, handleMouse);
	glfwSetWindowUserPointer(window, this);
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.f;
	return theChange;
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMove)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMove = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}
