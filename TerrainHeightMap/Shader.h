#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "GL/glew.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();
	
	GLuint shaderId;

	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvalPath);

	void useShader();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix4fv(const std::string& name, GLsizei count, const GLfloat* value) const;
	void setVec4(const std::string& name, float v1, float v2, float v3, float v4) const;
	void setVec3(const std::string& name, float v1, float v2, float v3) const;
	void setVec2(const std::string& name, float v1, float v2);
};

