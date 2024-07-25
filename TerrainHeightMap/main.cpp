#include <iostream>

#include "GL/glew.h"

#include "GLFW/glfw3.h"

#include "Shader.h"

int main()
{
	glfwInit();

	const int WIDTH = 1366;
	const int HEIGHT = 768;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "TerrainHeightMapWindow", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create window!\n";
		glfwTerminate();
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	glViewport(0, 0, WIDTH, HEIGHT);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialisation failed!" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return 2;
	}

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
		 0.0f,	0.5f, 0.0f,		0.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader shader("Shaders/shader.vert", "Shaders/shader.frag");
	shader.UseShader();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.2f, 0.3f, 0.3, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Wireframe mode
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}