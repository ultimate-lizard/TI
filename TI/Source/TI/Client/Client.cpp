#include "Client.h"

#include <filesystem>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Server.h>
#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Util/Config.h>
#include <TI/Server/Component/MeshComponent.h>
#include <TI/Server/Entity.h>
#include <TI/Renderer/Renderer.h>

LocalClient::LocalClient(Application* app) :
	Client(app)
{
	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(this, inputHandler.get());

	//if (app)
	//{
	//	auto& playerEntity = app->entities.at("player");
	//	playerController->posses(playerEntity.get());
	//}
	

	name = DEFAULT_PLAYER_NAME;

	loadConfig();
	loadMappings();
}

void LocalClient::connect()
{
	Server* server = app->getCurrentServer();
	if (server)
	{
		server->connectClient({ this });
	}
}

void LocalClient::receiveServerConnectionResponse(ServerConnectionResponse response)
{
	id = response.id;
}

void LocalClient::possesEntity(Entity* entity)
{
	Client::possesEntity(entity);

	playerController->posses(entity);
}

void LocalClient::update(float dt)
{
	// Render
	if (app)
	{
		Renderer* const renderer = app->getRenderer();
		if (renderer)
		{
			auto server = app->getCurrentServer();
			if (server)
			{
				for (auto& entityPair : server->getEntities())
				{
					auto& entity = entityPair.second;

					auto meshComp = entity->findComponent<MeshComponent>();
					if (meshComp)
					{
						renderer->pushRender(meshComp);
					}
				}
			}
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

void Client::setName(const std::string& name)
{
	this->name = name;
}

const std::string& Client::getName() const
{
	return name;
}

int Client::getId() const
{
	return id;
}

Application* const Client::getApplication() const
{
	return app;
}

void Client::possesEntity(Entity* entity)
{
	possessedEntity = entity;
}
