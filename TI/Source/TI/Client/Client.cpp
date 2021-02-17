#include "Client.h"

#include <TI/Application.h>
#include <TI/Server/Server.h>

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

void Client::shutdown()
{
	if (app)
	{
		auto server = app->getCurrentServer();
		if (server)
		{
			server->disconnectClient(this);
		}
	}
}

Application* const Client::getApplication() const
{
	return app;
}

void Client::possesEntity(Entity* entity)
{
	possessedEntity = entity;
}
