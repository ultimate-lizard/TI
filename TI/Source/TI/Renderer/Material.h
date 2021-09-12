#pragma once

#include <glm/glm.hpp>

class Shader;
class Texture;

class Material
{
public:
	Material(Shader* shader, Texture* texture, const glm::vec4& color = glm::vec4(0.0f));

	void setShader(Shader* shader);
	void setTexture(Texture* texture);

	void setColor(const glm::vec4& color);

	Material(const Material&) = delete;
	Material(Material&&) = delete;

	Shader* getShader() const;
	Texture* getTexture() const;
	const glm::vec4& getColor() const;

private:
	Shader* shader;
	Texture* texture;
	glm::vec4 color;
};
