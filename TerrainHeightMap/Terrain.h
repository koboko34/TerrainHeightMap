#pragma once

#include <vector>

#include "GL/glew.h"

#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"

#ifndef STB
#define STB
#include "External/stb_image.h"
#endif // !STB

#include "Shader.h"

class Terrain
{
public:
	Terrain();
	~Terrain();

	void createTerrain(float nearPlane, float farPlane);
	void renderTerrain(glm::mat4 view);
	void clearTerrain();

private:
	std::vector<float> terrainVertices;
	GLuint terrainVAO, terrainVBO;
	GLuint heightMapTexture;
	Shader terrainShader;
	unsigned int rez = 20;

};

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
	clearTerrain();
}

inline void Terrain::createTerrain(float nearPlane, float farPlane)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/iceland_heightmap.png", &width, &height, &nrChannels, 0);

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

	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	terrainShader = Shader("Shaders/terrain.vs", "Shaders/terrain.fs", "Shaders/terrain.tcs", "Shaders/terrain.tes");
	terrainShader.useShader();
	terrainShader.setInt("heightMapTexture", 0);
	GLint maxTessLevel;
	glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
	terrainShader.setInt("MAX_TESS_LEVEL", maxTessLevel);

	GLint colorLocation = glGetUniformLocation(terrainShader.shaderId, "color");
	glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.3f, 0.5f, 0.1f)));

	glm::mat4 model(1.f);
	terrainShader.setMatrix4fv("model", 1, glm::value_ptr(model));

	glm::mat4 projection;
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	width = viewport[2];
	height = viewport[3];
	projection = glm::perspective(glm::radians(45.f), (float)width / (float)height, nearPlane, farPlane);
	terrainShader.setMatrix4fv("projection", 1, glm::value_ptr(projection));

	glGenTextures(1, &heightMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);			// commented out to disable height displacement temporarily
		//glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
}

inline void Terrain::renderTerrain(glm::mat4 view)
{
	terrainShader.useShader();
	terrainShader.setMatrix4fv("view", 1, glm::value_ptr(view));
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glBindVertexArray(terrainVAO);
	glDrawArrays(GL_PATCHES, 0, 4 * rez * rez);
}

inline void Terrain::clearTerrain()
{
}
