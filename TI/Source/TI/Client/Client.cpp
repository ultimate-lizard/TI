#include "Client.h"

#include <filesystem>
#include <fstream>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Server.h>
#include <TI/Application.h>
#include <TI/Client/Input/InputHandler.h>
#include <TI/Client/Controller.h>

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
	std::ofstream file(configFilePath, std::fstream::app);
}

void LocalClient::loadMappings()
{
	// TODO: The mappings must be taken from config
	inputHandler->addAxisMapping(Key::W, AxisMapping({ "ForwardMovementAxis", 1.0f }));
	inputHandler->addAxisMapping(Key::S, AxisMapping({ "ForwardMovementAxis", -1.0f }));
	inputHandler->addAxisMapping(Key::A, AxisMapping({ "SideMovementAxis", -1.0f }));
	inputHandler->addAxisMapping(Key::D, AxisMapping({ "SideMovementAxis", 1.0f }));
	inputHandler->addAxisMapping(Axis::ControllerLeftStickX, AxisMapping({ "SideMovementAxis", 1.0f }));
	inputHandler->addAxisMapping(Axis::ControllerLeftStickY, AxisMapping({ "ForwardMovementAxis", -1.0f }));
	inputHandler->addAxisMapping(Axis::MouseX, AxisMapping({ "HorizontalLook", 1.0f }));
	inputHandler->addAxisMapping(Axis::MouseY, AxisMapping({ "VerticalLook", 1.0f }));
	inputHandler->addAxisMapping(Axis::ControllerRightStickX, AxisMapping({ "HorizontalLookRate", 1.0f }));
	inputHandler->addAxisMapping(Axis::ControllerRightStickY, AxisMapping({ "VerticalLookRate", -1.0f }));

	inputHandler->addKeyMapping(Key::ControllerButtonB, "QuitGame");
	inputHandler->addKeyMapping(Key::Escape, "QuitGame");
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
