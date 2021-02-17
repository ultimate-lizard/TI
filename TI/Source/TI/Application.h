#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <TI/Window.h>
#include <TI/SplitScreenManager.h>

// TODO: Move this to Window class
static const int DEFAULT_WINDOW_WIDTH = 800;
static const int DEFAULT_WINDOW_HEIGHT = 600;

static const char* CONFIG_FOLDER = "../Config/";
static const char* CONFIG_EXTENSION = ".cfg";

class Renderer;
class Input;
class Server;
class Client;
class Entity;
class LocalClient;
class ModelManager;

struct SDL_Window;

class Application
{
public:
	Application();
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;

	void start();
	void requestQuit();

	Window* getWindow();

	Server* const getCurrentServer() const;
	std::vector<LocalClient*> getLocalClients() const;

	Renderer* const getRenderer() const;
	ModelManager* const getModelManager() const;

private:
	void init();
	void initClients();
	void uninit();

private:
	bool simulating;

	Window window;

	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<ModelManager> modelManager;

	std::unique_ptr<Input> input;

	std::unique_ptr<Server> server;
	std::vector<std::unique_ptr<Client>> clients;

	SplitScreenManager splitScreenManager;
};
