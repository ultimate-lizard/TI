#pragma once

#include <string>

class Texture
{
public:
	Texture() : id(0), width(0), height(0) {};
	Texture(const std::string& path);
	~Texture();

	void create(int width, int height);
	void load(const std::string& path);
	
	void bind();
	void unbind();

	void resize();

	unsigned int getId() const;

private:
	unsigned int id;
	int width;
	int height;
};
