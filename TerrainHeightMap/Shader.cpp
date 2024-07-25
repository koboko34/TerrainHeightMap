#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (const std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ!" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	GLuint vShader, fShader;
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vShaderCode, NULL);
	glCompileShader(vShader);

	GLint success;
	char infoLog[512];
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vShader, sizeof(infoLog), NULL, infoLog);
		std::cout << "Vertex shader compilation failed!\n" << infoLog << std::endl;
	}

	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderCode, NULL);
	glCompileShader(fShader);

	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fShader, sizeof(infoLog), NULL, infoLog);
		std::cout << "Fragment shader compilation failed!\n" << infoLog << std::endl;
	}

	shaderId = glCreateProgram();
	glAttachShader(shaderId, vShader);
	glAttachShader(shaderId, fShader);
	glLinkProgram(shaderId);

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "Shader linking failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Shader::UseShader()
{
	glUseProgram(shaderId);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderId, name.c_str()), (GLint)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderId, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderId, name.c_str()), value);
}
