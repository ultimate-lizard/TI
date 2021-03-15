#include "RemoteServer.h"

#include <iostream>

#include <TI/Client/Client.h>
#include <TI/Server/Component/TransformComponent.h>
#include <TI/Application.h>

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

void RemoteServer::admitClient(Client* client)
{
	server = network.connect("localhost", 25565);
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

		createPlayerEntity(client->getName());
		possesEntity(client->getName(), client);
	}
}

void RemoteServer::update(float dt)
{
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

	auto entity = createEntity(name, id);
	auto transformComp = entity->findComponent<TransformComponent>();
	if (transformComp)
	{
		transformComp->setPosition(position);
		transformComp->setPitch(rotation.x);
		transformComp->setYaw(rotation.y);
		transformComp->setRoll(rotation.z);
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
			auto transformComp = entity->findComponent<TransformComponent>();
			if (transformComp)
			{
				glm::vec3 position;
				glm::vec3 rotation;
				packet >> position >> rotation;

				transformComp->setPosition(position);
				transformComp->setPitch(rotation.x);
				transformComp->setYaw(rotation.y);
				transformComp->setRoll(rotation.z);
			}
		}
	}
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

	auto transformComp = entity->findComponent<TransformComponent>();
	if (!transformComp)
	{
		return;
	}
		
	glm::vec3 position = transformComp->getPosition();

	glm::vec3 rotation;
	rotation.x = transformComp->getPitch();
	rotation.y = transformComp->getYaw();
	rotation.z = transformComp->getRoll();

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
