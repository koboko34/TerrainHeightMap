#pragma once

#include "GL/glew.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#ifndef STB
#define STB
#include "External/stb_image.h"
#endif // !STB

#include "Shader.h"

class BillboardGrass
{
public:
	BillboardGrass();
	~BillboardGrass();

	void createGrass(glm::mat4 projection);
	void renderGrass(glm::mat4 view);

private:
	GLuint VAO, VBO, EBO;
	GLuint grassTexture, noiseTexture;
	Shader shader;

	int BILLBOARDS_PER_GRASS = 3;
	const int NUM_GRASS_OBJECTS = 100 * 100;				// must keep this number square for now
	const float FIELD_DISTANCE = 100.f;
};

BillboardGrass::BillboardGrass()
{
}

BillboardGrass::~BillboardGrass()
{
}

inline void BillboardGrass::createGrass(glm::mat4 projection)
{
	glGenTextures(1, &grassTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/billboardgrass.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	stbi_image_free(data);

	glGenTextures(1, &noiseTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Textures/noiseTexture.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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

	BILLBOARDS_PER_GRASS = 3;									// must be same as below
	glm::mat4 grassModels[3];									// have to hard code for some reason
	grassModels[0] = glm::mat4(1.f);
	grassModels[1] = glm::rotate(grassModels[0], glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));
	grassModels[2] = glm::rotate(grassModels[0], glm::radians(240.f), glm::vec3(0.f, 1.f, 0.f));

	shader = Shader("Shaders/grass.vs", "Shaders/grass.fs");
	shader.useShader();
	shader.setInt("grassTexture", 1);
	shader.setInt("noiseTexture", 2);
	shader.setInt("NUM_GRASS_OBJECTS", NUM_GRASS_OBJECTS);
	shader.setInt("BILLBOARDS_PER_GRASS", BILLBOARDS_PER_GRASS);
	shader.setFloat("FIELD_DISTANCE", FIELD_DISTANCE);
	shader.setMatrix4fv("projection", 1, glm::value_ptr(projection));
	shader.setMatrix4fv("models", 3, &grassModels[0][0][0]);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(grassIndices), grassIndices, GL_STATIC_DRAW);
}

inline void BillboardGrass::renderGrass(glm::mat4 view)
{
	glBindVertexArray(VAO);
	shader.useShader();
	shader.setMatrix4fv("view", 1, glm::value_ptr(view));
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL, BILLBOARDS_PER_GRASS * NUM_GRASS_OBJECTS);
	glBindVertexArray(0);
}
