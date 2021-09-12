#include "Material.h"

Material::Material(Shader* shader, Texture* texture, const glm::vec4& color) :
	shader(shader),
	texture(texture),
	color(color)
{
}

void Material::setShader(Shader* shader)
{
	this->shader = shader;
}

void Material::setTexture(Texture* texture)
{
	this->texture = texture;
}

void Material::setColor(const glm::vec4& color)
{
	this->color = color;
}

Shader* Material::getShader() const
{
	return shader;
}

Texture* Material::getTexture() const
{
	return texture;
}

const glm::vec4& Material::getColor() const
{
	return color;
}
