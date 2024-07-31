#include <iostream>
#include <vector>
#include <chrono>

#include "GL/glew.h"

#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "Window.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"

#ifndef STB
#define STB
#include "External/stb_image.h"
#endif // !STB

int main()
{
	glfwInit();

	const int WIDTH = 1920;
	const int HEIGHT = 1080;
	const float FAR_PLANE = 5000.f;

	Window mainWindow(WIDTH, HEIGHT, false);
	Camera camera(mainWindow.getWindow());

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialisation failed!" << std::endl;
		return 2;
	}

	// ====================== TERRAIN =============================

	Terrain terrain;
	terrain.createTerrain(FAR_PLANE);

	// ==================== GRASS ======================

	GLuint grassTexture;
	glGenTextures(1, &grassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/billboardgrass.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	float grassVertices[] = {
		-1.f, 0.f,		0.f, 0.f,
		 1.f, 0.f,		1.f, 0.f,
		 1.f, 1.f,		1.f, 1.f,
		-1.f, 1.f,		0.f, 1.f
	};

	unsigned int grassIndices[] = {
		0, 1, 3,
		1, 2, 3
	};

	const int BILLBOARDS_PER_GRASS = 3;
	const int NUM_GRASS_OBJECTS = 60 * 60;
	const float FIELD_DISTANCE = 100.f;
	glm::mat4 grassModels[BILLBOARDS_PER_GRASS];
	grassModels[0] = glm::mat4(1.f);
	grassModels[1] = glm::rotate(grassModels[0], glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));
	grassModels[2] = glm::rotate(grassModels[0], glm::radians(240.f), glm::vec3(0.f, 1.f, 0.f));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, 0.1f, FAR_PLANE);

	Shader grassShader("Shaders/grass.vs", "Shaders/grass.fs");
	grassShader.UseShader();
	grassShader.setInt("grassTexture", 1);
	grassShader.setInt("NUM_GRASS_OBJECTS", NUM_GRASS_OBJECTS);
	grassShader.setFloat("FIELD_DISTANCE", FIELD_DISTANCE);
	grassShader.setMatrix4fv("projection", 1, glm::value_ptr(projection));
	grassShader.setMatrix4fv("models", 3, &grassModels[0][0][0]);

	GLuint grassVAO, grassVBO, grassEBO;
	glGenVertexArrays(1, &grassVAO);
	glBindVertexArray(grassVAO);

	glGenBuffers(1, &grassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &grassEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassIndices), grassIndices, GL_STATIC_DRAW);

	// ======================== PLAYER REFERENCE =================================

	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};

	GLuint playerReferenceVAO, playerReferenceVBO;
	glGenVertexArrays(1, &playerReferenceVAO);
	glBindVertexArray(playerReferenceVAO);

	glGenBuffers(1, &playerReferenceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, playerReferenceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	Shader basicShader("Shaders/shader.vs", "Shaders/shader.fs");
	basicShader.UseShader();
	glm::mat4 model;
	model = glm::mat4(1.f);
	model = glm::scale(model, glm::vec3(1.f, 2.f, 1.f));
	model = glm::translate(model, glm::vec3(-2.f, 0.5f, 0.f));
	basicShader.setMatrix4fv("model", 1, glm::value_ptr(model));
	basicShader.setMatrix4fv("projection", 1, glm::value_ptr(projection));

	std::chrono::duration<double> deltaTime;
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(mainWindow.getWindow()))
	{		
		auto currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		//std::cout << "Frame time: " << deltaTime.count() * 1000.0 << "ms" << std::endl;
		//std::cout << "FPS: " << 1.0 / deltaTime.count() << std::endl;
		
		glfwPollEvents();
		camera.processKeyInput(deltaTime.count());
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		view = camera.calculateViewMatrix();

		// Wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		terrain.renderTerrain(view);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(playerReferenceVAO);
		basicShader.UseShader();
		basicShader.setMatrix4fv("view", 1, glm::value_ptr(view));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(grassVAO);
		grassShader.UseShader();
		grassShader.setMatrix4fv("view", 1, glm::value_ptr(view));
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, BILLBOARDS_PER_GRASS * NUM_GRASS_OBJECTS);
		glBindVertexArray(0);

		glfwSwapBuffers(mainWindow.getWindow());
	}

	return 0;
}
