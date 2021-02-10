#include "Application.h"

#include <stdexcept>
#include <iostream>

#include <SDL.h>

#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Texture.h>
#include <TI/Renderer/Camera.h>
#include <TI/Client/InputHandler.h>
#include <TI/Client/Controller.h>
#include <TI/Client/Input.h>
#include <TI/Server/Entity.h>
#include <TI/Server/Component/Component.h>

// static const char* CONFIG_PATH = "Config.cfg";
static const char* GAME_TITLE = "TI";

static const float DELTA_MODIFIER = 0.0001f;

Application::Application() :
	simulating(false),
	sdlWindow(nullptr)
{
	SDL_SetMainReady();
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

	Camera camera({ 0.0f, 0.0f, 1.0f });
	camera.setPerspective(
		glm::radians(85.0f),
		static_cast<float>(DEFAULT_WINDOW_WIDTH) / static_cast<float>(DEFAULT_WINDOW_HEIGHT),
		0.01f,
		1000.0f
	);

	Entity entity;
	entity.addComponent<TransformComponent>(&entity);

	entity.addComponent<CameraComponent>(&entity);
	auto cameraComponent = entity.findComponent<CameraComponent>();
	cameraComponent->setCamera(&camera);

	entity.addComponent<MovementComponent>(&entity);

	playerController->posses(&entity);

	// -------------------------------------------------------------------------------

	int startFrame = 0;
	int endFrame = 0;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);

	while (simulating)
	{
		startFrame = SDL_GetTicks();

		input->handleInput();

		entity.tick(endFrame * DELTA_MODIFIER);

		renderer->render(&mesh, &shader, &camera);
		SDL_GL_SwapWindow(sdlWindow);

		endFrame = SDL_GetTicks() - startFrame;
	}
}

void Application::stop()
{
	simulating = false;
}

InputHandler* const Application::getInputHandler() const
{
	return inputHandler.get();
}

SDL_Window* const Application::getSDLWindow() const
{
	return sdlWindow;
}

void Application::init()
{
	renderer = std::make_unique<Renderer>(sdlWindow);
	renderer->setClearColor({0.0f, 0.0f, 0.0f, 1.0f});

	input = std::make_unique<Input>(this);
	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(inputHandler.get());
}
