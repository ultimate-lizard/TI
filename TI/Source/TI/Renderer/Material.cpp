#include "Material.h"

Material::Material() :
	shader(std::make_unique<Shader>()),
	texture(std::make_unique<Texture>())
{
}

void Material::setShader(const std::string& vertShaderSrc, const std::string& fragShaderSrc)
{
	shader->load(vertShaderSrc, fragShaderSrc);
}

void Material::setTexture(const std::string& path)
{
	texture->load(path);
}

void Material::setColor(const glm::vec4& color)
{
	this->color = color;
}

Shader* const Material::getShader()
{
	return shader.get();
}

Texture* const Material::getTexture()
{
	return texture.get();
}

const glm::vec4& Material::getColor() const
{
	return color;
}
