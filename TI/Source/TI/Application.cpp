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

namespace CmdArgHandles
{
	
}

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
	parseCmdArgs(args);
}

Application::~Application()
{
	SDL_Quit();
}

void Application::parseCmdArgs(const std::vector<std::string>& args)
{
	const char argPrefix {'-'};

	for (size_t i = 0; i < args.size(); ++i)
	{
		std::string word = args[i];

		CmdArg arg;

		if (*word.cbegin() != argPrefix)
		{
			continue;
		}
		
		word.erase(word.begin());
		arg.command = word;

		for (size_t j = i + 1; j < args.size(); ++j)
		{
			const std::string potentialArg = args[j];
			if (*args[i + 1].cbegin() != '-')
			{
				arg.args.push_back(potentialArg);
			}
			else
			{
				break;
			}
		}

		if (!arg.command.empty())
		{
			this->args.push_back(arg);
		}
	}
}

std::optional<CmdArg> Application::findCmdArg(const std::string& commandName)
{
	if (auto foundIter = std::find_if(args.begin(), args.end(), [commandName](const CmdArg& arg) { return arg.command == commandName; }); foundIter != args.end())
	{
		return *foundIter;
	}
}

void Application::execSplitscreenCmdArg()
{
	std::optional<CmdArg> cmdArgOptional = findCmdArg("splitscreen");

	if (!cmdArgOptional.has_value())
	{
		return;
	}

	CmdArg cmdArg = cmdArgOptional.value();

	for (size_t i = 0; i < cmdArg.args.size(); ++i)
	{
		std::string clName = cmdArg.args[i];

		auto localCl = std::make_unique<LocalClient>(this, clName);

		if (i == 0)
		{
			splitScreenManager.setHost(localCl.get());
		}
		else
		{
			splitScreenManager.addGuest(localCl.get());
			localCl->setViewportId(i);
		}

		clients.push_back(std::move(localCl));
	}

	if (cmdArg.args.size() > 1)
	{
		splitScreenManager.displayAll();
	}

	splitScreenManager.setOrientation(SplitScreenOrientation::Horizontal);
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
	renderer = std::make_unique<Renderer>(&window);

	resourceManager = std::make_unique<ResourceManager>();

	input = std::make_unique<Input>(this);

	execSplitscreenCmdArg();
	if (findCmdArg())

	if (!server)
	{
		server = std::make_unique<LocalServer>(this);
	}

	if (!getLocalClients().size())
	{
		clients.push_back(std::make_unique<LocalClient>(this));
	}

	for (LocalClient* locCl : getLocalClients())
	{
		if (locCl)
		{
			locCl->connect("", 0);
		}
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
