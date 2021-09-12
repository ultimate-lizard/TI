#pragma once

#include <memory>
#include <map>
#include <string>

#include <TI/Renderer/Model.h>
#include <TI/Renderer/Material.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Texture.h>

class ResourceManager
{
public:
	ResourceManager();
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;

	Texture* getTexture(const std::string& name) const;
	Shader* getShader(const std::string& name) const;
	Material* getMaterial(const std::string& name) const;
	Model* const getModel(const std::string& name) const;

private:
	void initResouces();

	Texture* createTexture(const std::string& name, const std::string& path);
	Shader* createShader(const std::string& name, const std::string& vertexShaderSourcePath, const std::string& fragmentShaderSourcePath);
	Material* createMaterial(const std::string& name, Shader* shader, Texture* texture, const glm::vec4 color = glm::vec4(0.0f));
	Model* createModel(const std::string& name, std::unique_ptr<Mesh> mesh, Material* material);

	std::map<std::string, std::unique_ptr<Texture>> textures;
	std::map<std::string, std::unique_ptr<Shader>> shaders;
	std::map<std::string, std::unique_ptr<Material>> materials;
	std::map<std::string, std::unique_ptr<Model>> models;
};
