#include "ResourceManager.h"

#include <TI/Renderer/Mesh.h>

ResourceManager::ResourceManager()
{
	initResouces();
}

Model* ResourceManager::createModel(const std::string& name, std::unique_ptr<Mesh> mesh, Material* material)
{
	if (const auto foundModel = models.find(name); foundModel == models.end())
	{
		return models.emplace(name, std::make_unique<Model>(std::move(mesh), material)).first->second.get();
	}
	else
	{
		throw std::exception();
	}
}

Model* const ResourceManager::getModel(const std::string& name) const
{
	if (const auto foundModel = models.find(name); foundModel != models.end())
	{
		return foundModel->second.get();
	}

	return nullptr;
}

Shader* ResourceManager::createShader(const std::string& name, const std::string& vertexShaderSourcePath, const std::string& fragmentShaderSourcePath)
{
	if (const auto foundShader = shaders.find(name); foundShader == shaders.end())
	{
		return shaders.emplace(name, std::make_unique<Shader>(vertexShaderSourcePath, fragmentShaderSourcePath)).first->second.get();
	}
	else
	{
		throw std::exception();
	}
}

Shader* ResourceManager::getShader(const std::string& name) const
{
	if (const auto foundShader = shaders.find(name); foundShader != shaders.end())
	{
		return foundShader->second.get();
	}

	return nullptr;
}

void ResourceManager::initResouces()
{
	MeshBuilder meshBuilder;

	meshBuilder.setPositions({
		// front
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		// back
		glm::vec3(-0.5f,  0.5f,  -0.5f),
		glm::vec3(0.5f,  0.5f,   -0.5f),
		glm::vec3(0.5f, -0.5f,   -0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		// left
		glm::vec3(-0.5f,  0.5f,   0.5f),
		glm::vec3(-0.5f,  0.5f,  -0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		glm::vec3(-0.5f, -0.5f,   0.5f),
		// right
		glm::vec3(0.5f,  0.5f,  -0.5f),
		glm::vec3(0.5f,  0.5f,   0.5f),
		glm::vec3(0.5f, -0.5f,   0.5f),
		glm::vec3(0.5f, -0.5f,  -0.5f),
		// top
		glm::vec3(0.5f, 0.5f,   0.5f),
		glm::vec3(0.5f, 0.5f,  -0.5f),
		glm::vec3(-0.5f, 0.5f,  -0.5f),
		glm::vec3(-0.5f, 0.5f,   0.5f),
		// bottom
		glm::vec3(0.5f,  -0.5f,  -0.5f),
		glm::vec3(0.5f,  -0.5f,   0.5f),
		glm::vec3(-0.5f, -0.5f,   0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		});

	meshBuilder.setIndices({
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
		});

	meshBuilder.setUVs({
		// front
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		// back
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		// left
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		// right
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		// top
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		// bottom
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		});

	std::unique_ptr<Mesh> cubeMesh = meshBuilder.build();

	Shader* defaultShader = createShader("Default", "../Shaders/Common.vert", "../Shaders/Common.frag");
	// Shader* planetShader = createShader("DistantPlanet", "../Shaders/DistantPlanet.vert", "../Shaders/DistantPlanet.frag");

	Texture* containerTexture = createTexture("Container", "../Textures/container.jpg");
	Material* containerMaterial = createMaterial("Container", defaultShader, containerTexture);
	createModel("Cube", std::move(cubeMesh), containerMaterial);

	Texture* planetTexture = createTexture("Planet", "../Textures/planet.jpg");
	Material* planetMaterial = createMaterial("DistantPlanet", defaultShader, planetTexture);
	// planetMaterial->setColor({ 0.14f, 0.13f, 0.10f, 0.8f });
	cubeMesh = meshBuilder.build();
	createModel("Planet", std::move(cubeMesh), planetMaterial);

	planetTexture = createTexture("Star", "../Textures/star.jpg");
	planetMaterial = createMaterial("Star", defaultShader, planetTexture);
	// planetMaterial->setColor({ 0.14f, 0.13f, 0.10f, 0.8f });
	cubeMesh = meshBuilder.build();
	createModel("Star", std::move(cubeMesh), planetMaterial);

	planetTexture = createTexture("GasGiant", "../Textures/gasgiant.jpg");
	planetMaterial = createMaterial("GasGiant", defaultShader, planetTexture);
	// planetMaterial->setColor({ 0.14f, 0.13f, 0.10f, 0.8f });
	cubeMesh = meshBuilder.build();
	createModel("GasGiant", std::move(cubeMesh), planetMaterial);

	Texture* playerTexture = createTexture("Player", "../Textures/player.jpg");
	Material* playerMaterial = createMaterial("Player", defaultShader, playerTexture);
	std::unique_ptr<Mesh> playerMesh = meshBuilder.build(); // This might cause issues
	createModel("Player", std::move(playerMesh), playerMaterial);

	Shader* chunkShader = createShader("Chunk", "../Shaders/Chunk.vert", "../Shaders/Chunk.frag");
	Texture* chunkTexture = createTexture("Chunk", "../Textures/dirt.jpg");
	createMaterial("Chunk", chunkShader, chunkTexture);
}

Texture* ResourceManager::createTexture(const std::string& name, const std::string& path)
{
	if (const auto foundTexture = textures.find(name); foundTexture == textures.end())
	{
		return textures.emplace(name, std::make_unique<Texture>(path)).first->second.get();
	}
	else
	{
		throw std::exception();
	}
}

Texture* ResourceManager::getTexture(const std::string& name) const
{
	if (const auto foundTexture = textures.find(name); foundTexture != textures.end())
	{
		return foundTexture->second.get();
	}

	return nullptr;
}

Material* ResourceManager::createMaterial(const std::string& name, Shader* shader, Texture* texture, const glm::vec4 color)
{
	if (const auto foundMaterial = materials.find(name); foundMaterial == materials.end())
	{
		return materials.emplace(name, std::make_unique<Material>(shader, texture, color)).first->second.get();
	}
	else
	{
		throw std::exception();
	}
}

Material* ResourceManager::getMaterial(const std::string& name) const
{
	if (const auto foundMaterial = materials.find(name); foundMaterial != materials.end())
	{
		return foundMaterial->second.get();
	}

	return nullptr;
}
