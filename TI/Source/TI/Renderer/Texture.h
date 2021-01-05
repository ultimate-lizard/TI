#pragma once

#include <string>

class Texture
{
public:
	Texture() : id(0) {};
	Texture(const std::string& path);

	void load(const std::string& path);

	void bind();
	void unbind();

private:
	unsigned int id;
};