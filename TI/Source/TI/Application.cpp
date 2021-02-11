#include "Application.h"

#include <stdexcept>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Camera.h>
#include <TI/Input.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Server/Component/CameraComponent.h>
#include <TI/Server/Component/MovementComponent.h>
#include <TI/Server/Server.h>
#include <TI/Client/Client.h>
#include <TI/Client/Controller.h>

// static const char* CONFIG_PATH = "Config.cfg";
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

	// Temp OpenGL -------------------------------------------------------------------

	Mesh mesh;

	mesh.setPositions({
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),

		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),

		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),

		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),

		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),

		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
	});

	mesh.setUVs({
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),

		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	});

	mesh.finalize();

	Shader shader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	shader.setVector("triangleColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	Texture texture("../Textures/container.jpg");
	texture.bind();

	auto camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 1.0f));
	camera->setPerspective(
		glm::radians(85.0f),
		static_cast<float>(DEFAULT_WINDOW_WIDTH) / static_cast<float>(DEFAULT_WINDOW_HEIGHT),
		0.01f,
		1000.0f
	);

	// -------------------------------------------------------------------------------

	entity = std::make_unique<Entity>();

	entity->addComponent<TransformComponent>(entity.get(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f));

	entity->addComponent<CameraComponent>(entity.get());
	auto cameraComponent = entity->findComponent<CameraComponent>();
	cameraComponent->setCamera(std::move(camera));

	entity->addComponent<MovementComponent>(entity.get());

	int startFrame = 0;
	int endFrame = 0;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);

	if (clients[0])
	{
		clients[0]->connect();
	}

	auto localClients = getLocalClients();
	localClients[0]->getController()->posses(entity.get());

	while (simulating)
	{
		startFrame = SDL_GetTicks();
		float dt = endFrame * DELTA_MODIFIER;

		input->handleInput();

		entity->tick(dt);
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

		glm::mat4 meshTransform = glm::mat4(1.0f);
		meshTransform = glm::translate(meshTransform, { 0.0f, 0.0f, -1.0f });
		meshTransform = glm::rotate(meshTransform, glm::radians(-45.0f), { 0.0f, 1.0f, 0.0f });

		renderer->render(&mesh, meshTransform, &shader, cameraComponent->getCamera());
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

void Application::init()
{
	renderer = std::make_unique<Renderer>(sdlWindow);
	renderer->setClearColor({0.0f, 0.0f, 0.0f, 1.0f});

	input = std::make_unique<Input>(this);

	server = std::make_unique<LocalServer>(this);
}

void Application::initClients()
{
	// TODO: Load clients from files
	// Pass them their configs

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
