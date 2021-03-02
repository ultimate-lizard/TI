#include "RemoteServer.h"

#include <iostream>

#include <TI/Client/Client.h>
#include <TI/Server/Component/TransformComponent.h>

RemoteServer::RemoteServer(Application* app) :
	Server(app),
	shuttingDown(false),
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
		server.receive(response);

		if (response.getPacketId() == PacketId::SConnectionResponse)
		{
			std::cout << "Received connection response" << std::endl;
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

		}
		break;

	default:
		std::cout << "Client is out of sync" << std::endl;
		throw std::exception();
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

	spawnedEntities.emplace(id, createEntity(name, id));
}

void RemoteServer::handleFinishInitialEntitySync(NetworkPacket& packet)
{
	state = RemoteServerState::Play;

	NetworkPacket response;
	response.setPacketId(PacketId::CFinishInitialEntitySync);
	server.send(response);
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
				std::cout << "Exception during RemoteClient::waitForMessages" << std::endl;
				server.close();
				shuttingDown = true;
				break;
			}
		}
	}
}
