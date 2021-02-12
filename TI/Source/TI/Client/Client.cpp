#include "Client.h"

#include <filesystem>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Server.h>
#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Util/Config.h>

LocalClient::LocalClient(Application* app) :
	Client(app)
{
	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(inputHandler.get());

	playerController->posses(app->entity.get());

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

void LocalClient::update(float dt)
{
	
}

InputHandler* const LocalClient::getInputHandler() const
{
	return inputHandler.get();
}

IController* const LocalClient::getController() const
{
	return playerController.get();
}

void LocalClient::loadConfig()
{
	std::string configFolderPath = CONFIG_FOLDER + std::string("Players/");
	if (!std::filesystem::exists(configFolderPath))
	{
		std::filesystem::create_directories(configFolderPath);
	}

	std::string configFilePath = configFolderPath + name + CONFIG_EXTENSION;
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
