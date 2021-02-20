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

	if (waitForMessageThread.joinable())
	{
		waitForMessageThread.join();
	}
}

void RemoteServer::connectClient(ClientConnectionRequest info)
{
	Socket server = network.connect("localhost", 25565);
	if (server)
	{
		ClientConnectionRequestMessage msg;
		msg.clientId = 123;
		msg.clientName = info.client->getName();

		char data[512];
		msg.serialize(data);

		if (!server.send(data, 512))
		{
			std::cout << "Unable to send the message to server" << std::endl;
			return;
		}

		connectedClients.emplace(info.client->getName(), info.client);
		createPlayerEntity(info.client->getName());
		possesEntity(info.client->getName(), info.client);

		waitForMessageThread = std::thread(&RemoteServer::waitForMessage, this, server);
	}
}

void RemoteServer::update(float dt)
{
	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}
}

void RemoteServer::waitForMessage(Socket socket)
{
	while (!shuttingDown)
	{
		char data[512];
		socket.receive(data, 512);
		EntityInfoMessage msg;
		msg.deserialzie(data);

		for (auto& mapPair : connectedClients)
		{
			auto& client = mapPair.second;
			if (client->getName() == msg.id)
			{
				continue;
			}
		}

		spawnedEntities.emplace(msg.id, createEntity(msg.name, msg.id));

		auto& entity = spawnedEntities[msg.id];
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			transformComp->setPosition({ msg.x, msg.y, msg.z });
			transformComp->setPitch(msg.pitch);
			transformComp->setYaw(msg.yaw);
			transformComp->setRoll(msg.roll);
		}
	}
}
