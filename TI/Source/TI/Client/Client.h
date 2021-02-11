#pragma once

#include <memory>
#include <string>

#include <TI/Server/ServerConnectionResponse.h>

class Application;
class InputHandler;
class IController;

static const char* DEFAULT_PLAYER_NAME = "Player";

class Client
{
public:
	Client(Application* app) : app(app), id(0) {}
	virtual ~Client() = default;

	virtual void update(float dt) {};

	void setName(const std::string& name);
	const std::string& getName() const;

	int getId() const;

	virtual void connect() = 0;
	virtual void receiveServerConnectionResponse(ServerConnectionResponse response) = 0;

	virtual void shutdown() {}

protected:
	Application* app;

	std::string name;
	int id;
};

class LocalClient : public Client
{
public:
	LocalClient(Application* app);

	void connect() override;
	void receiveServerConnectionResponse(ServerConnectionResponse response) override;

	void update(float dt) override;

	InputHandler* const getInputHandler() const;
	IController* const getController() const;

private:
	void loadConfig();
	void loadMappings();

	std::unique_ptr<InputHandler> inputHandler;
	std::unique_ptr<IController> playerController;
};
