#pragma once

#include <map>
#include <string>
#include <memory>
#include <atomic>

#include <TI/Client/ClientConnectionRequest.h>
#include <TI/Server/Entity.h>

class Application;
class Plane;

class Server
{
public:
	Server(Application* app);
	virtual ~Server();

	virtual void update(float dt);

	virtual void connectClient(Client* client, const std::string& ip, int port) = 0;
	virtual void ejectClient(Client* client) = 0;

	void requestShutdown();
	virtual void shutdown();

	Entity* const findEntity(const std::string& id);

	void removeEntity(const std::string& id);

	const std::map<std::string, std::unique_ptr<Entity>>& getEntities() const;

	std::unique_ptr<Entity> createEntityFromTemplate(const std::string& name, const std::string& id);

	const std::vector<std::unique_ptr<Plane>>& getPlanes() const;
	Entity* const spawnEntity(const std::string& templateName, const std::string& id, const glm::vec3& position);

protected:
	void initEntityTemplates();
	void possesEntity(const std::string& entityName, Client* const client);

protected:
	Application* app;

	std::map<std::string, std::unique_ptr<Entity>> spawnedEntities;
	std::map<std::string, std::unique_ptr<Entity>> entityTemplates;

	std::atomic<bool> shuttingDown;

	std::vector<std::unique_ptr<Plane>> planes;
};
