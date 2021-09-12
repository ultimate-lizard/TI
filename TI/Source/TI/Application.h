#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>

#include <TI/Window.h>
#include <TI/SplitScreenManager.h>

static const char* CONFIG_FOLDER = "../Config/";
static const char* CONFIG_EXTENSION = ".cfg";

class Renderer;
class Input;
class Server;
class Client;
class Entity;
class LocalClient;
class ResourceManager;

struct SDL_Window;

class Application
{
public:
	Application();
	Application(const std::vector<std::string>& args);
	~Application();

	Application(const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator=(const Application&) = delete;

	void start();
	void requestQuit();

	Window* getWindow();

	Server* const getCurrentServer() const;
	std::vector<LocalClient*> getLocalClients() const;
	const std::vector<std::unique_ptr<Client>>& getClients() const;

	Renderer* getRenderer() const;
	ResourceManager* getResourceManager() const;

	void addClient(std::unique_ptr<Client> client);
	void removeClient(const std::string& name);
	Client* findClient(const std::string& name);

	SplitScreenManager& getSplitScreenManager();

	Input* const getInput() const;

private:
	void init();

	void uninit();

	const std::vector<std::string>& getArgs() const;

private:
	bool simulating;

	Window window;

	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<ResourceManager> modelManager;

	std::unique_ptr<Input> input;

	std::unique_ptr<Server> server;
	std::vector<std::unique_ptr<Client>> clients;

	SplitScreenManager splitScreenManager;

	std::vector<std::string> args;
};
