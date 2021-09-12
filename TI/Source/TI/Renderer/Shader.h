#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

class Shader
{
	struct Uniform
	{
		std::string name;
		unsigned int location;
	};

public:
	Shader() : id(0) {}
	Shader(const std::string& vertexCodePath, const std::string& fragmentCodePath);
	~Shader();

	void load(const std::string& vertexCodePath, const std::string& fragmentCodePath);
	void use();

	void setVector(const std::string& name, const glm::vec4& value);
	void setMatrix(const std::string& name, const glm::mat4& value);

private:
	unsigned int getUniformLocation(const std::string& name);

private:
	unsigned int id;

	std::vector<Uniform> uniforms;
};
