#include "Texture.h"

#include <filesystem>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& path) :
	id(0)
{
	load(path);
}

Texture::~Texture()
{

}

void Texture::create(int width, int height)
{
	this->width = width;
	this->height = height;

	load("");
}

void Texture::load(const std::string& path)
{
	glGenTextures(1, &id);

	bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (path.empty() || !std::filesystem::exists(path))
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
	else
	{
		int width = 0;
		int height = 0;
		int channelCount = 0;

		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);
		this->width = width;
		this->height = height;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
	}

	unbind();
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::resize()
{
	// TODO
}

unsigned int Texture::getId() const
{
	return id;
}
