#include <iostream>
#include <vector>

#include "GL/glew.h"

#include "GLFW/glfw3.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

#include "Window.h"
#include "Camera.h"
#include "Shader.h"

int main()
{
	glfwInit();

	const int WIDTH = 1920;
	const int HEIGHT = 1080;

	Window mainWindow(WIDTH, HEIGHT, false);
	Camera camera(mainWindow.getWindow());

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialisation failed!" << std::endl;
		return 2;
	}

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/iceland_heightmap.png", &width, &height, &nrChannels, 0);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	std::vector<float> terrainVertices;
	unsigned int rez = 20;
	for (unsigned int i = 0; i <= rez - 1; i++)
	{
		for (unsigned int j = 0; j <= rez - 1; j++)
		{
			terrainVertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			terrainVertices.push_back(0.0f); // v.y
			terrainVertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			terrainVertices.push_back(i / (float)rez); // u
			terrainVertices.push_back(j / (float)rez); // v

			terrainVertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			terrainVertices.push_back(0.0f); // v.y
			terrainVertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			terrainVertices.push_back((i + 1) / (float)rez); // u
			terrainVertices.push_back(j / (float)rez); // v

			terrainVertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			terrainVertices.push_back(0.0f); // v.y
			terrainVertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			terrainVertices.push_back(i / (float)rez); // u
			terrainVertices.push_back((j + 1) / (float)rez); // v

			terrainVertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			terrainVertices.push_back(0.0f); // v.y
			terrainVertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			terrainVertices.push_back((i + 1) / (float)rez); // u
			terrainVertices.push_back((j + 1) / (float)rez); // v
		}
	}

	GLuint terrainVAO, terrainVBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	Shader terrainShader("Shaders/terrain.vs", "Shaders/terrain.fs", "Shaders/terrain.tcs", "Shaders/terrain.tes");
	terrainShader.UseShader();
	terrainShader.setInt("heightMapTexture", 0);
	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	terrainShader.setInt("MAX_TESS_LEVEL", maxTessLevel);

	glm::mat4 model(1.f);
	terrainShader.setMatrix4fv("model", model);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.f), (float)HEIGHT / (float)WIDTH, 0.1f, 5000.f);
	terrainShader.setMatrix4fv("projection", projection);

	GLuint heightMapTexture;
	glGenTextures(1, &heightMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);

	GLuint grassTexture;
	glGenTextures(1, &grassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = nullptr;
	data = stbi_load("Textures/billboardgrass.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);

	float grassVertices[] = {
		-1.f, -1.f,		0.f, 0.f,
		 1.f, -1.f,		1.f, 0.f,
		 1.f,  1.f,		1.f, 1.f,
		-1.f,  1.f,		0.f, 1.f
	};

	unsigned int grassIndices[] = {
		0, 1, 3,
		1, 2, 3
	};

	float grassOffsets[] = {
		 0.f, 0.f, 0.f,
		 5.f, 0.f, 0.f,
		10.f, 0.f, 0.f
	};

	const unsigned int BILLBOARDS_PER_GRASS = 3;
	glm::mat4 grassModels[BILLBOARDS_PER_GRASS];
	grassModels[0] = glm::mat4(1.f);
	grassModels[1] = glm::rotate(grassModels[0], glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));
	grassModels[2] = glm::rotate(grassModels[0], glm::radians(240.f), glm::vec3(0.f, 1.f, 0.f));

	Shader grassShader("Shaders/grass.vs", "Shaders/grass.fs");
	grassShader.UseShader();
	grassShader.setInt("grassTexture", 1);
	grassShader.setMatrix4fv("projection", projection);
	
	GLint grassModelsLocation = glGetUniformLocation(grassShader.shaderId, "models");
	glUniformMatrix4fv(grassModelsLocation, 3, GL_FALSE, &grassModels[0][0][0]);

	GLuint grassVAO, grassVBO, grassOffsetsVBO, grassEBO;
	glGenVertexArrays(1, &grassVAO);
	glBindVertexArray(grassVAO);

	glGenBuffers(1, &grassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &grassOffsetsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassOffsetsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassOffsets), grassOffsets, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 3);

	glGenBuffers(1, &grassEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassIndices), grassIndices, GL_STATIC_DRAW);


	while (!glfwWindowShouldClose(mainWindow.getWindow()))
	{
		glfwPollEvents();
		camera.processKeyInput();
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		glm::mat4 view;
		view = camera.calculateViewMatrix();
		terrainShader.UseShader();
		terrainShader.setMatrix4fv("view", view);

		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//glBindVertexArray(terrainVAO);
		//terrainShader.UseShader();
		//glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
		
		glBindVertexArray(grassVAO);
		grassShader.UseShader();
		grassShader.setMatrix4fv("view", view);
		//glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, BILLBOARDS_PER_GRASS * 3);
		//glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glBindVertexArray(0);

		glfwSwapBuffers(mainWindow.getWindow());
	}

	return 0;
}
