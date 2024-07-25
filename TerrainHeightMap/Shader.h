#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL/glew.h"

class Shader
{
public:
	GLuint shaderId;

	Shader(const char* vertexPath, const char* fragmentPath);

	void UseShader();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
};

