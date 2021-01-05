#include "Application.h"

#include <stdexcept>

#include <SDL.h>

#include <TI/Renderer/Renderer.h>
#include <TI/Renderer/Mesh.h>
#include <TI/Renderer/Shader.h>
#include <TI/Renderer/Texture.h>

static const char* CONFIG_PATH = "Config.cfg";
static const char* GAME_TITLE = "TI";

static const int DEFAULT_WINDOW_WIDTH = 800;
static const int DEFAULT_WINDOW_HEIGHT = 600;

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
		glm::vec3(0.5f, 0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.5f, 0.0f)
	});

	mesh.setUVs({
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f)
	});

	mesh.setIndices({
		0, 1, 3,
		1, 2, 3
	});

	mesh.finalize();

	Shader shader("../Shaders/SampleShader.vert", "../Shaders/SampleShader.frag");
	shader.use();
	shader.setVector("triangleColor", glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));

	Texture texture("../Textures/container.jpg");
	texture.bind();

	// -------------------------------------------------------------------------------

	SDL_Event Event;
	while (simulating)
	{
		while (SDL_PollEvent(&Event))
		{
			if (Event.type == SDL_QUIT)
			{
				simulating = false;
			}
		}

		renderer->render();
		SDL_GL_SwapWindow(sdlWindow);
	}
}

void Application::init()
{
	renderer = std::make_unique<Renderer>(sdlWindow);
}
