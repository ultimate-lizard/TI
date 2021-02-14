#pragma once

#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Shader.h>

class Material
{
public:
	void setShader(const std::string& vertShaderSrc, const std::string& fragShaderSrc);
	void setTexture(const std::string& path);

	Shader* const getShader();
	Texture* const getTexture();

private:
	Shader shader;
	Texture texture;
};
