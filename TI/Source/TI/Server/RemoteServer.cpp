#include "RemoteServer.h"

#include <iostream>

#include <TI/Client/Client.h>
#include <TI/Application.h>
#include <TI/Server/Component/TransformComponent.h>

RemoteServer::RemoteServer(Application* app) :
	Server(app),
	state(RemoteServerState::Undefined)
{
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

void RemoteServer::connectClient(Client* client, const std::string& ip, int port)
{
	server = network.connect(ip, port);
	if (server)
	{
		state = RemoteServerState::Handshake;

		NetworkPacket request(PacketId::CConnectionRequest);
		request << client->getName();

		server.send(request);

		NetworkPacket response;
		try
		{
			server.receive(response);
		}
		catch (std::exception&)
		{
			std::cout << "Unable to join the server" << std::endl;
			requestShutdown();
		}
	
		if (response.getPacketId() == PacketId::SConnectionResponse)
		{
			state = RemoteServerState::Sync;

			waitForMessageThread = std::thread(&RemoteServer::waitForMessages, this);
		}

		// spawnEntity(client->getName());
		possesEntity(client->getName(), client);
	}
}

void RemoteServer::update(float dt)
{
	Server::update(dt);

	for (auto& entityPair : spawnedEntities)
	{
		entityPair.second->tick(dt);
	}

	for (auto& client : app->getClients())
	{
		if (state == RemoteServerState::Play)
		{
			sendPlayerInfo(client.get());
		}
	}

	SDL_Delay(10);
}

void RemoteServer::shutdown()
{
	server.close();

	Server::shutdown();
}

void RemoteServer::handleMessage(NetworkPacket& packet)
{
	switch (packet.getPacketId())
	{
	case PacketId::SEntityInitialSync:
		if (state == RemoteServerState::Sync)
		{
			handleInitialEntitySync(packet);
		}
		break;

	case PacketId::SFinishInitialEntitySync:
	{
		if (state == RemoteServerState::Sync)
		{
			handleFinishInitialEntitySync(packet);
		}
	}
	break;

	case PacketId::SEntitySync:
		if (state == RemoteServerState::Play)
		{
			handleEntitySync(packet);
		}
		break;

	case PacketId::SSpawnPlayerEntity:
		if (state == RemoteServerState::Play)
		{
			handleSpawnPlayerEntity(packet);
		}
		break;

	case PacketId::SDestroyEntity:
		if (state == RemoteServerState::Play)
		{
			handleDestroyEntity(packet);
		}
		break;

	default:
		throw std::runtime_error("The client is out of sync");
	}
}

void RemoteServer::handleInitialEntitySync(NetworkPacket& packet)
{
	std::string name;
	std::string id;
	glm::vec3 position;
	glm::vec3 rotation;

	packet >> name;
	packet >> id;
	packet >> position >> rotation;

	std::unique_ptr<Entity> entity = createEntityFromTemplate(name, id);

	if (auto transformComponent = entity->findComponent<TransformComponent>())
	{
		transformComponent->setPosition(position);
		transformComponent->setRotation(rotation);
	}

	spawnedEntities.emplace(id, std::move(entity));
}

void RemoteServer::handleFinishInitialEntitySync(NetworkPacket& packet)
{
	state = RemoteServerState::Play;

	NetworkPacket response;
	response.setPacketId(PacketId::CFinishInitialEntitySync);
	server.send(response);
}

void RemoteServer::handleEntitySync(NetworkPacket& packet)
{
	std::string id;
	packet >> id;

	for (auto& mapPair : spawnedEntities)
	{
		auto& entity = mapPair.second;
		if (entity->getId() == id)
		{
			glm::vec3 position;
			glm::vec3 rotation;
			packet >> position >> rotation;

			if (auto transformComponent = entity->findComponent<TransformComponent>())
			{
				transformComponent->setPosition(position);
				transformComponent->setRotation(rotation);
			}
		}
	}
}

void RemoteServer::handleSpawnPlayerEntity(NetworkPacket& packet)
{
	std::string name;
	glm::vec3 position;
	glm::vec3 rotation;

	packet >> name >> position >> rotation;

	// spawnEntity(name);

	Entity* playerEntity = findEntity(name);
	if (playerEntity)
	{
		if (auto transformComponent = playerEntity->findComponent<TransformComponent>())
		{
			transformComponent->setPosition(position);
			transformComponent->setRotation(rotation);
		}
	}
}

void RemoteServer::handleDestroyEntity(NetworkPacket& packet)
{
	std::string id;
	packet >> id;

	removeEntity(id);
}

void RemoteServer::sendPlayerInfo(Client* client)
{
	auto server = app->getCurrentServer();
	if (!server)
	{
		return;
	}

	auto entity = server->findEntity(client->getName());
	if (!entity)
	{
		return;
	}
	
	if (auto transformComponent = entity->findComponent<TransformComponent>())
	{
		glm::vec3 position = transformComponent->getPosition();
		glm::vec3 rotation = transformComponent->getRotation();

		NetworkPacket packet;
		packet.setPacketId(PacketId::CPlayerSync);
		packet << client->getName() << position << rotation;

		try
		{
			this->server.send(packet);
		}
		catch (std::exception&)
		{
			requestShutdown();
		}
	}
}

void RemoteServer::waitForMessages()
{
	while (!shuttingDown)
	{
		if (server)
		{
			try
			{
				NetworkPacket packet;
				server.receive(packet);
				handleMessage(packet);
			}
			catch (std::exception&)
			{
				std::cout << "Lost connection to the remote server or an error occurred during package receipt" << std::endl;
				requestShutdown();
				break;
			}
		}
	}
}
