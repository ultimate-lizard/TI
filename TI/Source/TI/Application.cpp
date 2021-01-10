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

// static const char* CONFIG_PATH = "Config.cfg";
static const char* GAME_TITLE = "TI";

static const int DEFAULT_WINDOW_WIDTH = 1920;
static const int DEFAULT_WINDOW_HEIGHT = 1080;

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

	playerController->possesCamera(&camera);

	renderer->setClearColor({ 1.0f, 0.0f, 1.0f, 1.0f });

	// -------------------------------------------------------------------------------
	int startFrame = 0;
	int endFrame = 0;

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_CaptureMouse(SDL_TRUE);
	
	int lastX = 0;
	int lastY = 0;

	SDL_Event event;
	while (simulating)
	{
		startFrame = SDL_GetTicks();

		bool keyDown = false;
		int key = 0;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				simulating = false;
				break;

			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_KEYDOWN:
				inputHandler->onKeyInput(event.key.keysym.scancode, ActionInputType::KeyPress);
				inputHandler->onAxisInput(event.key.keysym.scancode, 1.0f);
				break;

			case SDL_CONTROLLERBUTTONUP:
			case SDL_KEYUP:
				inputHandler->onKeyInput(event.key.keysym.scancode, ActionInputType::KeyRelease);
				inputHandler->onAxisInput(event.key.keysym.scancode, 0.0f);
				break;

			case SDL_MOUSEMOTION:
				{
					int xPos = event.motion.x;
					int yPos = event.motion.y;

					float xOffset = static_cast<float>(xPos) - lastX;
					float yOffset = static_cast<float>(lastY) - yPos;

					lastX = xPos;
					lastY = yPos;

					xOffset = static_cast<float>(xOffset) / (127 - -127) * 2;
					yOffset = static_cast<float>(yOffset) / (127 - -127) * 2;

					inputHandler->onAxisInput(Axis::MouseX, static_cast<float>(xOffset));
					inputHandler->onAxisInput(Axis::MouseY, static_cast<float>(yOffset));

					SDL_WarpMouseInWindow(sdlWindow, DEFAULT_WINDOW_WIDTH / 2, DEFAULT_WINDOW_HEIGHT / 2);

					break;
				}
				

			case SDL_CONTROLLERAXISMOTION:
				{
					// TODO: Move that to config
					int deadZone = 3000;

					int motion = event.caxis.value;
					float normalizedMotion = 0.0f;

					if (motion < -deadZone || motion > deadZone)
					{
						normalizedMotion = static_cast<float>(motion) / (32767 - -32768) * 2;
					}

					switch (event.caxis.axis)
					{
					case SDL_CONTROLLER_AXIS_LEFTX:
						inputHandler->onAxisInput(Axis::ControllerLeftStickX, normalizedMotion);
						break;
					case SDL_CONTROLLER_AXIS_LEFTY:
						inputHandler->onAxisInput(Axis::ControllerLeftStickY, normalizedMotion);
						break;
					case SDL_CONTROLLER_AXIS_RIGHTX:
						inputHandler->onAxisInput(Axis::ControllerRightStickX, normalizedMotion);
						break;
					case SDL_CONTROLLER_AXIS_RIGHTY:
						inputHandler->onAxisInput(Axis::ControllerRightStickY, normalizedMotion);
						break;
					}

					break;
				}
			
				inputHandler->onKeyInput(event.key.keysym.scancode, ActionInputType::KeyPress);
				inputHandler->onAxisInput(event.key.keysym.scancode, 1.0f);
				break;
			}
		}

		camera.update(endFrame * 0.0001f);

		renderer->render(&mesh, &shader, &camera);
		SDL_GL_SwapWindow(sdlWindow);

		endFrame = SDL_GetTicks() - startFrame;
	}
}

void Application::init()
{
	renderer = std::make_unique<Renderer>(sdlWindow);
	inputHandler = std::make_unique<InputHandler>();
	playerController = std::make_unique<PlayerController>(inputHandler.get());
}
