#pragma once

#include <memory>
#include <string>

#include <TI/Server/ServerConnectionResponse.h>
#include <TI/Util/Config.h>

class Application;
class InputHandler;
class IController;
class Entity;

static const char* DEFAULT_PLAYER_NAME = "Player";

class Client
{
public:
	Client(Application* app) : app(app), id(0), possessedEntity(nullptr) {}
	virtual ~Client() = default;

	virtual void update(float dt) {};

	void setName(const std::string& name);
	const std::string& getName() const;

	int getId() const;

	virtual void connect() = 0;
	virtual void receiveServerConnectionResponse(ServerConnectionResponse response) = 0;

	virtual void shutdown() {}

	Application* const getApplication() const;

	virtual void possesEntity(Entity* entity);

protected:
	Application* app;

	std::string name;
	int id;

	Entity* possessedEntity;
};

class LocalClient : public Client
{
public:
	LocalClient(Application* app);

	void update(float dt) override;

	void connect() override;
	void receiveServerConnectionResponse(ServerConnectionResponse response) override;

	void possesEntity(Entity* entity) override;

	InputHandler* const getInputHandler() const;
	IController* const getController() const;

	void shutdown() override;

private:
	void loadConfig();
	void loadMappings();

	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;

	Config config;
};
