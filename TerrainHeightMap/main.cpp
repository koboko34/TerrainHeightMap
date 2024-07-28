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

	const int WIDTH = 800;
	const int HEIGHT = 600;

	Window mainWindow(WIDTH, HEIGHT);
	Camera camera(mainWindow.getWindow());

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialisation failed!" << std::endl;
		return 2;
	}

	/*
	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	*/

	// stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/iceland_heightmap.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// glGenerateMipmap(GL_TEXTURE_2D);			// doesn't render without. Double check when using as height info
	}

	std::vector<float> terrainVertices;
	float yScale = 64.f / 256.f;			// dividing by 256 puts the height values into [0, 1], multiplying by 64 allows for manual adjustment of scale
	float yShift = 0.f;						// allows control of height by translation up or down

	for (unsigned int i = 0; i < height; i++)
	{
		for (unsigned int j = 0; j < width; j++)
		{
			unsigned char* texel = data + (j + width * i) * nrChannels;		// retrieves texel at (i, j) tex coord
			unsigned char y = texel[0];										// store raw height

			terrainVertices.push_back(-height / 2.f + i);
			terrainVertices.push_back((int)y * yScale - yShift);
			terrainVertices.push_back(-width / 2.f + j);
		}
	}

	stbi_image_free(data);

	std::vector<unsigned int> terrainIndices;
	const unsigned int NUM_STRIPS = height - 1;
	const unsigned int NUM_VERTS_PER_STRIP = width * 2;
	for (unsigned int i = 0; i < NUM_STRIPS; i++)					// each triangle strip row
	{
		for (unsigned int j = 0; j < width; j++)					// each column
		{
			for (unsigned int k = 0; k < 2; k++)
			{
				terrainIndices.push_back(j + width * (i + k));		// for each side of strip (up and down)
			}
		}
	}

	GLuint terrainVAO, terrainVBO, terrainEBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size() * sizeof(float), terrainIndices.data(), GL_STATIC_DRAW);

	/*
	float terrainVertices[] = {
		-3.f, 0.f, -3.f,	0.f, 0.f,
		-3.f, 0.f,  3.f,	0.f, 1.f,
		 3.f, 0.f, -3.f,	1.f, 0.f,
		 3.f, 0.f,  3.f,	1.f, 1.f
	};
	*/

	/*
	unsigned int terrainIndices[] = {
		0, 1, 2,
		2, 3, 1
	};
	*/

	/*
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), terrainIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	*/

	GLuint heightMapTexture;
	glGenTextures(1, &heightMapTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMapTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*
	Shader shader("Shaders/shader.vert", "Shaders/shader.frag");
	shader.UseShader();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);
	*/

	Shader terrainShader("Shaders/terrain.vert", "Shaders/terrain.frag");
	terrainShader.UseShader();
	terrainShader.setInt("heightMapTexture", 0);

	glm::mat4 model(1.f);
	terrainShader.setMatrix4fv("model", model);

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.f), (float)HEIGHT / (float)WIDTH, 0.1f, 5000.f);
	terrainShader.setMatrix4fv("projection", projection);

	/*
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	*/

	while (!glfwWindowShouldClose(mainWindow.getWindow()))
	{
		glfwPollEvents();
		camera.processKeyInput();
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		
		glm::mat4 view;
		view = camera.calculateViewMatrix();
		terrainShader.setMatrix4fv("view", view);

		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Wireframe mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glBindVertexArray(terrainVAO);

		/*
		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model(1.f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.f, 0.3f, 0.5f));
			GLint modelLoc = glGetUniformLocation(shader.shaderId, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glm::mat4 view;
			view = camera.calculateViewMatrix();
			GLint viewLoc = glGetUniformLocation(shader.shaderId, "view");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		*/

		for (unsigned int strip = 0; strip < NUM_STRIPS; strip++)
		{
			glDrawElements(
				GL_TRIANGLE_STRIP,
				NUM_VERTS_PER_STRIP,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * NUM_VERTS_PER_STRIP * strip)
			);
		}

		// glDrawElements(GL_TRIANGLES, sizeof(terrainIndices), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(mainWindow.getWindow());
	}

	return 0;
}
