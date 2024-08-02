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
#include "BillboardGrass.h"

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
	const float NEAR_PLANE = 0.1;

	Window mainWindow(WIDTH, HEIGHT, false);
	Camera camera(mainWindow.getWindow());

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialisation failed!" << std::endl;
		return 2;
	}
	
	// ======================== SCREEN / FRAMEBUFFER ==============================

	GLuint FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error with framebuffer when trying to bind to depth attachment!" << std::endl;
	}

	GLuint colorTexture;
	glGenTextures(1, &colorTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error with framebuffer when trying to bind to color attachment!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader fogShader("Shaders/screen.vs", "Shaders/fog.fs");
	fogShader.useShader();
	fogShader.setInt("depthTexture", 3);
	fogShader.setInt("colorTexture", 4);
	fogShader.setFloat("near", NEAR_PLANE);
	fogShader.setFloat("far", FAR_PLANE);
	fogShader.setFloat("fogDensity", 0.05f);
	fogShader.setVec4("fogColor", 0.5f, 0.5f, 0.7f, 1.f);

	Shader boxBlurShader("Shaders/screen.vs", "Shaders/boxBlur.fs");
	boxBlurShader.useShader();
	boxBlurShader.setInt("colorTexture", 4);
	boxBlurShader.setInt("radius", 15);
	boxBlurShader.setVec2("texOffset", 1.f / (float)WIDTH, 1.f / (float)HEIGHT);

	float quadVertices[] = {
		-1.f, -1.f,		0.f, 0.f,
		 1.f, -1.f,		1.f, 0.f,
		 1.f,  1.f,		1.f, 1.f,
		-1.f,  1.f,		0.f, 1.f
	};

	unsigned int quadIndices[] = {
		0, 1, 3,
		1, 2, 3
	};

	GLuint quadVAO, quadVBO, quadEBO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &quadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// ====================== TERRAIN =============================

	glm::mat4 projection = glm::perspective(45.f, (float)WIDTH / (float)HEIGHT, NEAR_PLANE, FAR_PLANE);

	Terrain terrain;
	terrain.createTerrain(projection);

	// ==================== GRASS ======================

	BillboardGrass billboardGrass;
	billboardGrass.createGrass(projection);

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
	basicShader.useShader();
	glm::mat4 model;
	model = glm::mat4(1.f);
	model = glm::scale(model, glm::vec3(1.f, 2.f, 1.f));
	model = glm::translate(model, glm::vec3(0.f, 0.5f, 0.f));
	basicShader.setMatrix4fv("model", 1, glm::value_ptr(model));
	basicShader.setMatrix4fv("projection", 1, glm::value_ptr(projection));

	std::chrono::duration<double> deltaTime;
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(mainWindow.getWindow()))
	{		
		auto currentTime = std::chrono::high_resolution_clock::now();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		std::cout << "\rFrame time (ms): " << deltaTime.count() * 1000.0 << "          " << std::flush;
		// std::cout << "FPS: " << 1.0 / deltaTime.count() << std::endl;
		
		glfwPollEvents();
		camera.processKeyInput(deltaTime.count());
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		glm::mat4 view;
		view = camera.calculateViewMatrix();
		
		// first pass ========= drawing to off-screen framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.4f, 0.8f, 0.9f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		terrain.renderTerrain(view);

		glBindVertexArray(playerReferenceVAO);
		basicShader.useShader();
		basicShader.setMatrix4fv("view", 1, glm::value_ptr(view));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		billboardGrass.renderGrass(view);
		
		// second pass ======== drawing to default on-screen framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glBindVertexArray(quadVAO);
		//fogShader.useShader();
		boxBlurShader.useShader();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		glfwSwapBuffers(mainWindow.getWindow());
	}

	return 0;
}
