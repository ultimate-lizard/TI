#include "Application.h"

#include <stdexcept>

#include <TI/Renderer/Renderer.h>
#include <TI/Input.h>
#include <TI/Server/Server.h>
#include <TI/Client/Client.h>
#include <TI/ModelManager.h>

static const char* GAME_TITLE = "TI";

static const float DELTA_MODIFIER = 0.0001f;

Application::Application() :
	simulating(false),
	sdlWindow(nullptr)
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
	sdlWindow = SDL_CreateWindow(
		GAME_TITLE,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		DEFAULT_WINDOW_WIDTH,
		DEFAULT_WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
		| SDL_WINDOW_OPENGL
	);

	if (!sdlWindow)
	{
		// TODO: Handle this
		throw std::exception();
	}
}

Application::~Application()
{
	if (sdlWindow)
	{
		SDL_DestroyWindow(sdlWindow);
	}

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

	if (clients[0])
	{
		clients[0]->connect();
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

		SDL_GL_SwapWindow(sdlWindow);

		endFrame = SDL_GetTicks() - startFrame;
	}

	uninit();
}

void Application::requestQuit()
{
	simulating = false;
}

SDL_Window* const Application::getSDLWindow() const
{
	return sdlWindow;
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

void Application::init()
{
	renderer = std::make_unique<Renderer>(sdlWindow);
	renderer->setClearColor({0.0f, 0.0f, 0.0f, 1.0f});

	modelManager = std::make_unique<ModelManager>();

	input = std::make_unique<Input>(this);

	server = std::make_unique<LocalServer>(this);
}

void Application::initClients()
{
	clients.push_back(std::make_unique<LocalClient>(this));
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
