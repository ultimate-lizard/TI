#include "Application.h"

#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include <TI/Renderer/Renderer.h>
#include <TI/Input.h>
#include <TI/Server/LocalServer.h>
#include <TI/Server/RemoteServer.h>
#include <TI/Client/LocalClient.h>
#include <TI/Client/DebugInformation.h>
#include <TI/ResourceManager.h>

Application::Application() :
	simulating(false),
	splitScreenManager(this)
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		// TODO: Handle Error
		throw std::exception();
	}

	SDL_GameController* controller;
	if (int i = SDL_NumJoysticks() > 0)
	{
		if (SDL_IsGameController(0))
		{
			controller = SDL_GameControllerOpen(0);
		}
	}

	// TODO: Load config

	// TODO: Create window using config
	window.init(std::string(GAME_TITLE) + " - !!!!!!!!!!!!! THE GAME IS LOADING, PLEASE WAIT !!!!!!!!!!!!!");
}

Application::Application(const std::vector<std::string>& args) :
	Application()
{
	this->args = args;
}

Application::~Application()
{
	SDL_Quit();
}

void Application::start()
{
	init();

	simulating = true;

	uint64_t currentFrame = SDL_GetPerformanceCounter();
	uint64_t lastFrame = 0;

	const float DT = 0.0069f;
	float accumulator = 0.0f;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);

	while (simulating)
	{
		lastFrame = currentFrame;
		currentFrame = SDL_GetPerformanceCounter();
		
		float frameTime = (currentFrame - lastFrame) / static_cast<float>(SDL_GetPerformanceFrequency());
		
		input->handleInput();

		accumulator += frameTime;
		while (accumulator >= DT)
		{
			accumulator -= DT;

			if (server)
			{
				server->update(DT);
			}
		}

		for (auto& client : clients)
		{
			if (client)
			{
				if (client->isPendingDeletion())
				{
					removeClient(client->getName());
					continue;
				}

				client->update(frameTime);
			}
		}
		
		renderer->render();

		if (!getLocalClients().empty())
		{
			getLocalClients()[0]->getDebugInformation()->updateDebugMeshLifetime();
		}

		window.swap();

		if (clients.empty())
		{
			break;
		}
	}

	uninit();
}

void Application::requestQuit()
{
	simulating = false;
}

Window* Application::getWindow()
{
	return &window;
}

Server* const Application::getCurrentServer() const
{
	return server.get();
}

std::vector<LocalClient*> Application::getLocalClients() const
{
	std::vector<LocalClient*> localClients;

	for (auto& client : clients)
	{
		auto localClient = dynamic_cast<LocalClient*>(client.get());
		if (localClient)
		{
			localClients.push_back(localClient);
		}
	}

	return localClients;
}

const std::vector<std::unique_ptr<Client>>& Application::getClients() const
{
	return clients;
}

Renderer* Application::getRenderer() const
{
	return renderer.get();
}

ResourceManager* Application::getResourceManager() const
{
	return resourceManager.get();
}

void Application::addClient(std::unique_ptr<Client> client)
{
	clients.push_back(std::move(client));
}

void Application::removeClient(const std::string& name)
{
	auto iter = std::find_if(clients.begin(), clients.end(), [&name](std::unique_ptr<Client>& client) {
		return client->getName() == name;
	});

	if (iter != clients.end())
	{
		(*iter)->shutdown();
		clients.erase(iter);
	}
}

Client* Application::findClient(const std::string& name)
{
	auto iter = std::find_if(clients.begin(), clients.end(), [&name](auto& client) {
		return client->getName() == name;
	});

	return iter != clients.end() ? iter->get() : nullptr;
}

SplitScreenManager& Application::getSplitScreenManager()
{
	return splitScreenManager;
}

Input* const Application::getInput() const
{
	return input.get();
}

void Application::init()
{
	// parse args

	renderer = std::make_unique<Renderer>(&window);

	resourceManager = std::make_unique<ResourceManager>();

	input = std::make_unique<Input>(this);

	server = std::make_unique<LocalServer>(this);

	clients.push_back(std::make_unique<LocalClient>(this));
	auto client2 = std::make_unique<LocalClient>(this, "Player2");
	client2->setViewportId(1);

	splitScreenManager.setHost(dynamic_cast<LocalClient*>(clients[0].get()));
	splitScreenManager.addGuest(client2.get());
	splitScreenManager.setOrientation(SplitScreenOrientation::Horizontal);

	clients.push_back(std::move(client2));

	splitScreenManager.displayAll();

	getLocalClients().at(0)->connect("", 0);
	getLocalClients().at(1)->connect("", 0);
}

void Application::uninit()
{
	for (auto& client : clients)
	{
		client->shutdown();
	}

	if (server)
	{
		server->shutdown();
	}
}

const std::vector<std::string>& Application::getArgs() const
{
	return args;
}
