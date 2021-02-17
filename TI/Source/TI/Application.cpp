#include "Application.h"

#include <stdexcept>
#include <algorithm>

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

	for (auto& client : clients)
	{
		client->connect();
	}

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
				client->update(dt);
			}
		}

		renderer->render();

		window.swap();

		endFrame = SDL_GetTicks() - startFrame;
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

void Application::init()
{
	renderer = std::make_unique<Renderer>(&window);
	renderer->setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	modelManager = std::make_unique<ModelManager>();

	input = std::make_unique<Input>(this);

	 
	if (args.size() >= 2)
	{
		if (args[1] == "client")
		{
			server = std::make_unique<RemoteServer>(this);
		}

		if (args[1] == "server")
		{
			server = std::make_unique<ListenServer>(this);
		}
	}
}

void Application::initClients()
{
	auto player1 = std::make_unique<LocalClient>(this);

	if (!args.empty())
	{
		player1->setName(args[0]);
	}

	//auto player2 = std::make_unique<LocalClient>(this, "Player2");
	//player2->setViewportId(1);

	//auto player3 = std::make_unique<LocalClient>(this, "Player3");
	//player3->setViewportId(2);

	//splitScreenManager.setHost(player1.get());
	//splitScreenManager.addGuest(player2.get());
	//splitScreenManager.addGuest(player3.get());

	clients.push_back(std::move(player1));
	//clients.push_back(std::move(player2));
	//clients.push_back(std::move(player3));

	//splitScreenManager.setOrientation(SplitScreenOrientation::Horizontal);

	//splitScreenManager.displayHost();
	//splitScreenManager.displayAll();
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
