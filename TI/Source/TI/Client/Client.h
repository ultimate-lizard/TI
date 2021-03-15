#pragma once

#include <memory>
#include <string>
#include <atomic>

#include <TI/Server/ServerConnectionResponse.h>

enum class ClientState
{
	Undefined,
	Sync,
	Play
};

class Application;
class Entity;

static const char* DEFAULT_PLAYER_NAME = "Player";

class Client
{
public:
	Client(Application* app);
	virtual ~Client() = default;

	virtual void update(float dt);

	void setName(const std::string& name);
	const std::string& getName() const;

	int getId() const;

	virtual void connect() = 0;

	void requestShutdown();
	virtual void shutdown();

	Application* const getApplication() const;

	virtual void possesEntity(Entity* entity);

	ClientState getState() const;

	bool isPendingDeletion() const;

protected:
	Application* app;

	std::string name;
	int id;

	Entity* possessedEntity;

	ClientState state;

	bool pendingDeletion;
	std::atomic<bool> shuttingDown;
};
