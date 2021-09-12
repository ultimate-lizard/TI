#include "LocalClient.h"

#include <filesystem>
#include <functional>

#include <glm/gtc/matrix_transform.hpp>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Server.h>
#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Util/Config.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Material.h>
#include <TI/Renderer/Mesh.h>
#include <TI/ModelManager.h>
#include <TI/Client/ChunkMesh.h>
#include <TI/Server/Plane.h>
#include <TI/Util/Random.h>

LocalClient::LocalClient(Application* app, const std::string& name) :
	Client(app),
	viewportId(0)
{
	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(this, inputHandler.get());

	this->name = name.empty() ? DEFAULT_PLAYER_NAME : name;

	loadConfig();
	loadMappings();
}

LocalClient::~LocalClient()
{
	for (auto& mesh : chunkMeshes)
	{
		if (mesh)
		{
			delete mesh;
		}
	}

	chunkMeshes.clear();
}

void LocalClient::connect(const std::string& ip, int port)
{
	if (Server* server = app->getCurrentServer())
	{
		server->connectClient(this, ip, port);

		const std::vector<Chunk>& chunks = server->getPlane()->getChunks();
		for (unsigned int i = 0; i < chunks.size(); ++i)
		{
			chunkMeshes.push_back(new ChunkMesh(&chunks[i], server->getPlane()));
		}

		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f);
		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1.0f);
		drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0f);
	}
}

void LocalClient::setPossesedEntity(Entity* entity)
{
	Client::setPossesedEntity(entity);

	playerController->posses(entity);

	if (!entity)
	{
		return;
	}

	auto cameraComp = entity->findComponent<CameraComponent>();
	if (!cameraComp)
	{
		return;
	}

	auto renderer = app->getRenderer();
	if (!renderer)
	{
		return;
	}

	auto viewport = renderer->getViewport(viewportId);
	if (viewport)
	{
		viewport->setActiveCamera(cameraComp->getCamera());
	}
}

void LocalClient::update(float dt)
{
	for (int i : meshesToErase)
	{
		app->getModelManager()->removeModel(debugMeshes[i].name);
		debugMeshes.erase(debugMeshes.begin() + i);
	}

	meshesToErase.clear();

	Renderer* const renderer = app->getRenderer();
	if (!renderer)
	{
		return;
	}

	auto server = app->getCurrentServer();
	if (!server)
	{
		return;
	}

	// Render debug stuff
	for (int i = 0; i < debugMeshes.size(); ++i)
	{
		DebugMeshInfo& debugMeshInfo = debugMeshes[i];

		Model* model = app->getModelManager()->findModel(debugMeshInfo.name);

		if (model)
		{
			renderer->pushRender(model->getMesh(), model->getMaterial(), glm::mat4(1.0f), viewportId, debugMeshInfo.meshType, debugMeshInfo.width);
		}

		if (!debugMeshInfo.persistent)
		{
			meshesToErase.push_back(i);
		}
	}

	// Render world
	if (renderer)
	{
		for (ChunkMesh* chunkMesh : chunkMeshes)
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), chunkMesh->getPosition());
			renderer->pushRender(&chunkMesh->getMesh(), &chunkMesh->getMaterial(), transform, viewportId);
		}
	}
	
	// Render entities
	for (auto& entityPair : server->getEntities())
	{
		auto& entity = entityPair.second;

		auto meshComp = entity->findComponent<MeshComponent>();
		if (meshComp)
		{
			// Don't render your entity as we play in first person
			if (entity->getId() == name)
			{
				// continue;
			}

			renderer->pushRender(meshComp, viewportId);
		}
	}
}

InputHandler* const LocalClient::getInputHandler() const
{
	return inputHandler.get();
}

IController* const LocalClient::getController() const
{
	return playerController.get();
}

void LocalClient::shutdown()
{
	config.save();

	Client::shutdown();
}

void LocalClient::setViewportId(unsigned int id)
{
	viewportId = id;
}

unsigned int LocalClient::getViewportId() const
{
	return viewportId;
}

void LocalClient::drawDebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float width, bool persistent)
{
	Material mat;
	mat.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	mat.setColor(color);

	MeshBuilder meshBuilder;
	meshBuilder.setPositions({ start, end });

	Mesh mesh = meshBuilder.build();

	auto model = std::make_unique<Model>();
	model->setMesh(std::move(mesh));
	model->setMaterial(std::move(mat));

	std::string name = "DebugMesh" + random_string(10);

	app->getModelManager()->addModel(name, std::move(model));

	debugMeshes.push_back({ name, color, width, GL_LINES, persistent });
}

void LocalClient::drawDebugPoint(const glm::vec3& position, const glm::vec4& color, float width)
{
	Material mat;
	mat.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	mat.setColor(color);

	MeshBuilder meshBuilder;
	meshBuilder.setPositions({ position });

	Mesh mesh = meshBuilder.build();

	auto model = std::make_unique<Model>();
	model->setMesh(std::move(mesh));
	model->setMaterial(std::move(mat));

	std::string name = "DebugMesh" + random_string(10);

	app->getModelManager()->addModel(name, std::move(model));

	debugMeshes.push_back({ name, color, width, GL_POINTS, true });
}

void LocalClient::drawDebugBox(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float width, bool persistent)
{
	Material mat;
	mat.setShader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	mat.setColor(color);

	MeshBuilder meshBuilder;

	meshBuilder.setPositions({
		// front
		//glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		//glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		//glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		//glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		//glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		//glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		//glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		//glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		//glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		//glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		//glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		//glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		//glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		//glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		//glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		//glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		//glm::vec3( 0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		//glm::vec3( 0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		//glm::vec3(-0.5f + position.x,  0.5f + position.y, -0.5f + position.z),
		//glm::vec3(-0.5f + position.x,  0.5f + position.y,  0.5f + position.z),
		//glm::vec3( 0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
		//glm::vec3( 0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		//glm::vec3(-0.5f + position.x, -0.5f + position.y,  0.5f + position.z),
		//glm::vec3(-0.5f + position.x, -0.5f + position.y, -0.5f + position.z),
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

	Mesh mesh = meshBuilder.build();

	auto model = std::make_unique<Model>();
	model->setMesh(std::move(mesh));
	model->setMaterial(std::move(mat));

	std::string name = "DebugMesh" + random_string(10);

	app->getModelManager()->addModel(name, std::move(model));

	debugMeshes.push_back({ name, color, width, GL_LINES, persistent });
}

std::vector<ChunkMesh*>& LocalClient::getChunkMeshes()
{
	return chunkMeshes;
}

void LocalClient::loadConfig()
{
	std::string configFolderPath = CONFIG_FOLDER + std::string("Players/");
	std::string configFilePath = configFolderPath + name + CONFIG_EXTENSION;
	if (!std::filesystem::exists(configFolderPath))
	{
		std::filesystem::create_directories(configFolderPath);
	}

	if (!std::filesystem::exists(configFilePath))
	{
		std::filesystem::copy_file(CONFIG_FOLDER + std::string("Default.cfg"), configFilePath);
	}

	config.load(configFilePath);
}

void LocalClient::loadMappings()
{
	auto axisEntries = config.getAxisEntries();
	for (const auto& entry : axisEntries)
	{
		if (entry.axis == Axis::UnknownAxis)
		{
			inputHandler->addAxisMapping(entry.key, AxisMapping({ entry.bindingName, entry.scale }));
		}
		else
		{
			inputHandler->addAxisMapping(entry.axis, AxisMapping({ entry.bindingName, entry.scale }));
		}
	}

	auto keyEntries = config.getKeyEntries();
	for (const auto& entry : keyEntries)
	{
		inputHandler->addKeyMapping(entry.key, entry.bindingName);
	}
}
