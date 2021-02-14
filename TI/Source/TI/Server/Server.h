#pragma once

#include <map>
#include <string>
#include <memory>

#include <TI/Client/ClientConnectionRequest.h>

class Application;
class Entity;

class Server
{
public:
	Server(Application* app) : app(app) {}
	virtual ~Server() = default;

	virtual void update(float dt) {};

	virtual void connectClient(ClientConnectionRequest info) = 0;

	virtual void shutdown() {}

	Entity* const findEntity(const std::string& name);
	const std::map<std::string, std::unique_ptr<Entity>>& getEntities() const;

protected:
	Application* app;

	std::map<std::string, Client*> connectedClients;
	std::map<std::string, std::unique_ptr<Entity>> entities;
};

class LocalServer : public Server
{
public:
	LocalServer(Application* app);

	void update(float dt) override;

	void connectClient(ClientConnectionRequest request) override;

private:
	void createCubes();
	void createPlayerEntity(const std::string& name);
	void possesEntity(const std::string& entityName, const std::string& clientName);
};
