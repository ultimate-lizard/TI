#include "ModelManager.h"

#include <TI/Renderer/Model.h>
#include <TI/Renderer/Material.h>

ModelManager::ModelManager()
{
	Mesh mesh;

	mesh.setPositions({
		// front
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		// back
		glm::vec3(-0.5f,  0.5f,  -0.5f),
		glm::vec3(0.5f,  0.5f,   -0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		glm::vec3(0.5f,  0.5f,   -0.5f),
		glm::vec3(0.5f, -0.5f,   -0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),

		// left
		glm::vec3(-0.5f,  0.5f,   0.5f),
		glm::vec3(-0.5f,  0.5f,  -0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		glm::vec3(-0.5f,  0.5f,   0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		glm::vec3(-0.5f, -0.5f,   0.5f),

		// right
		glm::vec3(0.5f,  0.5f,  -0.5f),
		glm::vec3(0.5f,  0.5f,   0.5f),
		glm::vec3(0.5f, -0.5f,  -0.5f),
		glm::vec3(0.5f,  0.5f,   0.5f),
		glm::vec3(0.5f, -0.5f,   0.5f),
		glm::vec3(0.5f, -0.5f,  -0.5f),

		// top
		glm::vec3( 0.5f, 0.5f,   0.5f),
		glm::vec3( 0.5f, 0.5f,  -0.5f),
		glm::vec3(-0.5f, 0.5f,  -0.5f),
		glm::vec3( 0.5f, 0.5f,   0.5f),
		glm::vec3(-0.5f, 0.5f,  -0.5f),
		glm::vec3(-0.5f, 0.5f,   0.5f),

		// bottom
		glm::vec3(0.5f,  -0.5f,  -0.5f),
		glm::vec3(0.5f,  -0.5f,   0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		glm::vec3(0.5f,  -0.5f,   0.5f),
		glm::vec3(-0.5f, -0.5f,   0.5f),
		glm::vec3(-0.5f, -0.5f,  -0.5f),
		});

	mesh.setUVs({
		// front
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// back
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),

		// left
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// right
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),

		// top
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		// bottom
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
	});

	mesh.finalize();

	auto model = std::make_unique<Model>();
	model->setMesh(mesh);

	Material mat;
	mat.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	mat.setTexture("../Textures/container.jpg");

	model->setMaterial(std::move(mat));

	models.emplace("cube", std::move(model));

	auto playerModel = std::make_unique<Model>();
	playerModel->setMesh(std::move(mesh));

	Material playerMat;
	playerMat.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	playerMat.setTexture("../Textures/player.jpg");

	playerModel->setMaterial(std::move(playerMat));

	models.emplace("player", std::move(playerModel));
}

Model* const ModelManager::findModel(const std::string& name) const
{
	auto iter = models.find(name);
	if (iter != models.end())
	{
		return iter->second.get();
	}
	
	return nullptr;
}

void ModelManager::addModel(const std::string& name, std::unique_ptr<Model> model)
{
	models.emplace(name, std::move(model));
}
