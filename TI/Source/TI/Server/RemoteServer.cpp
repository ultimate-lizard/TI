#include "RemoteServer.h"

#include <iostream>

#include <TI/Network/Message.h>
#include <TI/Client/Client.h>
#include <TI/Server/Component/TransformComponent.h>

RemoteServer::RemoteServer(Application* app) :
	Server(app),
	shuttingDown(false)
{
	// network.connect("localhost")
	initEntityTemplates();
}

RemoteServer::~RemoteServer()
{
	shuttingDown = true;

	server.close();

	if (waitForMessageThread.joinable())
	{
		waitForMessageThread.join();
	}
}

void RemoteServer::connectClient(ClientConnectionRequest info)
{
	server = network.connect("localhost", 25565);
	if (server)
	{
		ClientConnectionRequestMessage msg;
		msg.clientName = info.client->getName();

		Buffer buffer(512);
		// msg.serialize(buff);
		serializeNetMsg(msg, buffer);

		if (!server.send(buffer, 512))
		{
			std::cout << "Unable to send the message to server" << std::endl;
			return;
		}

		connectedClients.emplace(info.client->getName(), info.client);
		createPlayerEntity(info.client->getName());
		possesEntity(info.client->getName(), info.client);

		waitForMessageThread = std::thread(&RemoteServer::waitForMessage, this);
	}
}

void RemoteServer::update(float dt)
{
	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}
}

void RemoteServer::waitForMessage()
{
	while (!shuttingDown)
	{
		Buffer buffer(512);
		server.receive(buffer, 512);

		//EntityInfoMessage msg;
		//msg.deserialzie(buffer);
		auto msg = deserializeNetMsg(buffer);
		if (auto entityInfoMsg = std::get_if<EntityInfoMessage>(&msg))
		{
			for (auto& mapPair : connectedClients)
			{
				auto& client = mapPair.second;
				if (client->getName() == entityInfoMsg->id)
				{
					continue;
				}
			}

			spawnedEntities.emplace(entityInfoMsg->id, createEntity(entityInfoMsg->name, entityInfoMsg->id));

			auto& entity = spawnedEntities[entityInfoMsg->id];
			auto transformComp = entity->findComponent<TransformComponent>();
			if (transformComp)
			{
				transformComp->setPosition({ entityInfoMsg->x, entityInfoMsg->y, entityInfoMsg->z });
				transformComp->setPitch(entityInfoMsg->pitch);
				transformComp->setYaw(entityInfoMsg->yaw);
				transformComp->setRoll(entityInfoMsg->roll);
			}
		}
	}
}
