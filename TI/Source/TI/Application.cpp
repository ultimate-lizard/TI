#include "Application.h"

#include <stdexcept>
#include <algorithm>
#include <sstream>

#include <TI/Renderer/Renderer.h>
#include <TI/Input.h>
#include <TI/Server/LocalServer.h>
#include <TI/Server/ListenServer.h>
#include <TI/Server/RemoteServer.h>
#include <TI/Client/LocalClient.h>
#include <TI/ModelManager.h>

static const char* GAME_TITLE = "TI";

static const float DELTA_MODIFIER = 0.0001f;

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
	if (SDL_NumJoysticks() > 0)
	{
		if (SDL_IsGameController(0))
		{
			controller = SDL_GameControllerOpen(0);
		}
	}

	// TODO: Load config

	// TODO: Create window using config
	window.init(GAME_TITLE);
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
	initClients();

	simulating = true;

	int startFrame = 0;
	int endFrame = 0;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);

	while (simulating)
	{
		startFrame = SDL_GetTicks();
		float dt = endFrame * DELTA_MODIFIER;

		input->handleInput();

		if (server)
		{
			server->update(dt);
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

				client->update(dt);
			}
		}

		renderer->render();

		window.swap();

		endFrame = SDL_GetTicks() - startFrame;

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

	return std::move(localClients);
}

const std::vector<std::unique_ptr<Client>>& Application::getClients() const
{
	return clients;
}

Renderer* const Application::getRenderer() const
{
	return renderer.get();
}

ModelManager* const Application::getModelManager() const
{
	return modelManager.get();
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
	renderer = std::make_unique<Renderer>(&window);
	renderer->setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	modelManager = std::make_unique<ModelManager>();

	input = std::make_unique<Input>(this);

	if (args.size() >= 3)
	{
		if (args[1] == "client")
		{
			server = std::make_unique<RemoteServer>(this);
		}

		if (args[1] == "server")
		{
			std::stringstream ss;
			ss << args[2];
			int port = 0;
			ss >> port;
			server = std::make_unique<ListenServer>(this, port);
		}
	}
	else
	{
		server = std::make_unique<LocalServer>(this);
	}
}

void Application::initClients()
{
	if (args.size() >= 2)
	{
		if (args[1] == "server")
		{
			auto player1 = std::make_unique<LocalClient>(this, args[0]);

			auto player3 = std::make_unique<LocalClient>(this, "Player3");
			player3->setViewportId(1);

			splitScreenManager.setHost(player1.get());
			splitScreenManager.addGuest(player3.get());

			splitScreenManager.displayAll();

			player1->connect("", 0);
			player3->connect("", 0);

			clients.push_back(std::move(player1));
			clients.push_back(std::move(player3));
		}
	}

	if (args.size() == 4)
	{
		if (args[1] == "client")
		{
			std::string ip = args[2];
			int port = 0;
			std::stringstream ss;
			ss << args[3];
			ss >> port;
			auto player1 = std::make_unique<LocalClient>(this);
			player1->setName(args[0]);
			player1->connect(ip, port);
			clients.push_back(std::move(player1));
		}
	}
	else
	{
		auto player1 = std::make_unique<LocalClient>(this);
		player1->connect("", 0);
		clients.push_back(std::move(player1));
	}
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
