#include "Material.h"

void Material::setShader(const std::string& vertShaderSrc, const std::string& fragShaderSrc)
{
	shader.load(vertShaderSrc, fragShaderSrc);
}

void Material::setTexture(const std::string& path)
{
	texture.load(path);
}

void Material::setColor(const glm::vec4& color)
{
	this->color = color;
}

Shader* const Material::getShader()
{
	return &shader;
}

Texture* const Material::getTexture()
{
	return &texture;
}

const glm::vec4& Material::getColor() const
{
	return color;
}
