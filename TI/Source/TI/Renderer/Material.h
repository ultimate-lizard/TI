#pragma once

#include <memory>

#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Shader.h>

class Material
{
public:
	Material();

	void setShader(const std::string& vertShaderSrc, const std::string& fragShaderSrc);
	void setTexture(const std::string& path);
	void setColor(const glm::vec4& color);

	Material(const Material&) = delete;
	Material(Material&&) = delete;

	Shader* const getShader();
	Texture* const getTexture();
	const glm::vec4& getColor() const;

private:
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Texture> texture;
	glm::vec4 color;
};
