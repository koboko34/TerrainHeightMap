#include "Shader.h"

Shader::Shader()
{
}

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

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvalPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string tControlCode;
	std::string tEvalCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream tControlFile;
	std::ifstream tEvalFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tControlFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tEvalFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		tControlFile.open(tessControlPath);
		tEvalFile.open(tessEvalPath);
		std::stringstream vShaderStream, fShaderStream, tControlStream, tEvalStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		tControlStream << tControlFile.rdbuf();
		tEvalStream << tEvalFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		tControlFile.close();
		tEvalFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		tControlCode = tControlStream.str();
		tEvalCode = tEvalStream.str();
	}
	catch (const std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ!" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* tControlShaderCode = tControlCode.c_str();
	const char* tEvalShaderCode = tEvalCode.c_str();

	GLuint vShader, fShader, tControlShader, tEvalShader;
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

	tControlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tControlShader, 1, &tControlShaderCode, NULL);
	glCompileShader(tControlShader);

	glGetShaderiv(tControlShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(tControlShader, sizeof(infoLog), NULL, infoLog);
		std::cout << "Tessellation control shader compilation failed!\n" << infoLog << std::endl;
	}

	tEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tEvalShader, 1, &tEvalShaderCode, NULL);
	glCompileShader(tEvalShader);

	glGetShaderiv(tEvalShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(tEvalShader, sizeof(infoLog), NULL, infoLog);
		std::cout << "Tessellation evaluation shader compilation failed!\n" << infoLog << std::endl;
	}

	shaderId = glCreateProgram();
	glAttachShader(shaderId, vShader);
	glAttachShader(shaderId, fShader);
	glAttachShader(shaderId, tControlShader);
	glAttachShader(shaderId, tEvalShader);
	glLinkProgram(shaderId);

	glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderId, sizeof(infoLog), NULL, infoLog);
		std::cout << "Shader linking failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);
	glDeleteShader(tControlShader);
	glDeleteShader(tEvalShader);
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

void Shader::setMatrix4fv(const std::string& name, GLsizei count, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderId, name.c_str()), count, GL_FALSE, value);
}
