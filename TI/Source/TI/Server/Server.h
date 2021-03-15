#pragma once

#include <map>
#include <string>
#include <memory>
#include <atomic>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Entity.h>

class Application;

class Server
{
public:
	Server(Application* app) : app(app), shuttingDown(false) {}
	virtual ~Server() = default;

	virtual void update(float dt) {};

	virtual void admitClient(Client* client) = 0;
	virtual void ejectClient(Client* client) = 0;

	void requestShutdown();
	virtual void shutdown();

	Entity* const findEntity(const std::string& id);
	const std::map<std::string, std::unique_ptr<Entity>>& getEntities() const;

	std::unique_ptr<Entity> createEntity(const std::string& name, const std::string& id);

protected:
	void initEntityTemplates();
	void createPlayerEntity(const std::string& name);
	void possesEntity(const std::string& entityName, Client* client);

protected:
	Application* app;

	std::map<std::string, std::unique_ptr<Entity>> spawnedEntities;
	std::map<std::string, std::unique_ptr<Entity>> entityTemplates;

	std::atomic<bool> shuttingDown;
};
