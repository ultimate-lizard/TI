#pragma once

#include <memory>
#include <string>

#include <TI/Server/ServerConnectionResponse.h>

class Application;
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
	virtual void shutdown();

	Application* const getApplication() const;

	virtual void possesEntity(Entity* entity);

protected:
	Application* app;

	std::string name;
	int id;

	Entity* possessedEntity;
};
