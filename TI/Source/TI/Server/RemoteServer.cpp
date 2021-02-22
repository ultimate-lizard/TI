#include "RemoteServer.h"

#include <iostream>

#include <TI/Network/Message.h>
#include <TI/Client/Client.h>
#include <TI/Server/Component/TransformComponent.h>

RemoteServer::RemoteServer(Application* app) :
	Server(app),
	shuttingDown(false),
	state(ServerState::ServerStateSync)
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

void RemoteServer::connectClient(Client* client)
{
	server = network.connect("93.78.45.19", 25565);
	// server = network.connect("localhost", 25565);
	if (server)
	{
		ClientConnectionRequestNetMessage msg;
		msg.clientName = client->getName();

		Buffer buffer(512);
		// msg.serialize(buff);
		serializeNetMessage(msg, buffer);

		if (!server.send(buffer, 512))
		{
			return;
		}

		if (!server.receive(buffer, 512))
		{
			return;
		}

		auto responseVariant = deserializeNetMessage(buffer);
		if (auto response = std::get_if<ClientConnectionResponseNetMessage>(&responseVariant))
		{
			if (response->clientName != client->getName())
			{
				return;
			}
		}
		else
		{
			return;
		}

		connectedClients.emplace(client->getName(), client);
		createPlayerEntity(client->getName());
		possesEntity(client->getName(), client);

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

		auto msg = deserializeNetMessage(buffer);
		if (auto entityInfoMsg = std::get_if<EntityInfoNetMessage>(&msg))
		{
			if (state == ServerState::ServerStateSync)
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
			else if (state == ServerState::ServerStatePlay)
			{
				bool skip = false;
				for (auto& mapPair : connectedClients)
				{
					auto& client = mapPair.second;
					if (client->getName() == entityInfoMsg->id)
					{
						skip = true;
					}
				}

				if (skip) continue;

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
		else if (std::get_if<FinishInitialEntitySyncNetMessage>(&msg))
		{
			// Receive sync messages
			state = ServerState::ServerStatePlay;

			Buffer buffer(512);
			serializeNetMessage(ClientReadyNetMessage(), buffer);
			server.send(buffer, 512);
		}
	}
}
