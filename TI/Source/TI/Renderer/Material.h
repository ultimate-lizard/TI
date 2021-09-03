#pragma once

#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Shader.h>

class Material
{
public:
	void setShader(const std::string& vertShaderSrc, const std::string& fragShaderSrc);
	void setTexture(const std::string& path);
	void setColor(const glm::vec4& color);

	Shader* const getShader();
	Texture* const getTexture();
	const glm::vec4& getColor() const;

private:
	Shader shader;
	Texture texture;
	glm::vec4 color;
};
