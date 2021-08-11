#include "Client.h"

#include <TI/Application.h>
#include <TI/Server/Server.h>

Client::Client(Application* app) :
	app(app),
	id(0),
	possessedEntity(nullptr),
	pendingDeletion(false),
	state(ClientState::Undefined),
	shuttingDown(false)
{

}

void Client::update(float dt)
{
	if (shuttingDown)
	{
		shutdown();
	}
}

void Client::setName(const std::string& name)
{
	this->name = name;
}

const std::string& Client::getName() const
{
	return name;
}

int Client::getId() const
{
	return id;
}

void Client::requestShutdown()
{
	shuttingDown = true;
}

void Client::shutdown()
{
	auto server = app->getCurrentServer();
	if (server)
	{
		server->ejectClient(this);
	}

	pendingDeletion = true;
}

ClientState Client::getState() const
{
	return state;
}

Application* const Client::getApplication() const
{
	return app;
}

void Client::setPossesedEntity(Entity* entity)
{
	possessedEntity = entity;
}

bool Client::isPendingDeletion() const
{
	return pendingDeletion;
}
