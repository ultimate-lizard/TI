#include "Shader.h"

#include <iostream>

#include <glad/glad.h>

#include <TI/Util/File.h>

Shader::Shader(const std::string& vertexCodePath, const std::string& fragmentCodePath) :
	id(0)
{
	load(vertexCodePath, fragmentCodePath);
}

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::load(const std::string& vertexCodePath, const std::string& fragmentCodePath)
{
	unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	id = glCreateProgram();

	int status = 0;
	char log[1024];

	std::string vertexCode = readFileToString(vertexCodePath);
	std::string fragmentCode = readFileToString(fragmentCodePath);

	const char* vertexCodeC = vertexCode.c_str();
	const char* fragmentCodeC = fragmentCode.c_str();
	glShaderSource(vertexShaderId, 1, &vertexCodeC, NULL);
	glShaderSource(fragmentShaderId, 1, &fragmentCodeC, NULL);

	glCompileShader(vertexShaderId);
	glCompileShader(fragmentShaderId);

	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderInfoLog(vertexShaderId, 1024, NULL, log);
		
		// TODO: Log to logger
		std::cout << "Vertex shader compilation error:" << std::endl << log << std::endl;
	}

	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		glGetShaderInfoLog(fragmentShaderId, 1024, NULL, log);

		// TODO: Log to logger
		std::cout << "Fragment shader compilation error:" << std::endl << log << std::endl;
	}

	glAttachShader(id, vertexShaderId);
	glAttachShader(id, fragmentShaderId);
	glLinkProgram(id);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	int uniformCount = 0;
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);
	uniforms.resize(uniformCount);

	// TODO: replace size and type to nullptr?
	char buffer[128];
	int size = 0;
	for (int i = 0; i < uniformCount; ++i)
	{
		GLenum type;
		glGetActiveUniform(id, i, sizeof(buffer), 0, &size, &type, buffer);
		uniforms[i].name = buffer;
		uniforms[i].location = glGetUniformLocation(id, buffer);
	}
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setVector4(const std::string& name, const glm::vec4& value)
{
	int location = getUniformLocation(name);
	if (location != -1)
	{
		glUniform4fv(location, 1, &value[0]);
	}
}

void Shader::setVector3(const std::string& name, const glm::vec3& value)
{
	int location = getUniformLocation(name);
	if (location != -1)
	{
		glUniform3fv(location, 1, &value[0]);
	}
}

void Shader::setMatrix(const std::string& name, const glm::mat4& value)
{
	int location = getUniformLocation(name);
	if (location != -1)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}
}

void Shader::setFloat(const std::string& name, float value)
{
	int location = getUniformLocation(name);
	if (location != -1)
	{
		glUniform1f(location, value);
	}
}

unsigned int Shader::getUniformLocation(const std::string& name)
{
	for (const auto& uniform : uniforms)
	{
		if (uniform.name == name)
		{
			return uniform.location;
		}
	}

	return -1;
}
