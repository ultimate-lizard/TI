#include "RemoteClient.h"

#include <iostream>

#include <TI/Application.h>
#include <TI/Server/Server.h>
#include <TI/Server/Component/TransformComponent.h>

RemoteClient::RemoteClient(Application* app) :
	Client(app),
	shuttingDown(false)
{
	messageThread = std::thread(&RemoteClient::waitForMessages, this);
}

void RemoteClient::shutdown()
{
	socket.close();
	if (messageThread.joinable())
	{
		messageThread.join();
	}

	Client::shutdown();
}

void RemoteClient::update(float dt)
{
	if (shuttingDown)
	{
		shutdown();
	}
}

void RemoteClient::setSocket(Socket socket)
{
	this->socket = socket;
}

Socket RemoteClient::getSocket() const
{
	return socket;
}

void RemoteClient::waitForMessages()
{
	state = ClientState::Sync;

	while (!shuttingDown)
	{
		if (socket)
		{
			try
			{
				NetworkPacket packet;
				socket.receive(packet);
				handleMessage(packet);
			}
			catch (std::exception&)
			{
				std::cout << "Exception during RemoteClient::waitForMessages" << std::endl;
				socket.close();
				shuttingDown = true;
				break;
			}
		}
	}
}

void RemoteClient::handleFinishInitialEntitySync(NetworkPacket& packet)
{
	std::cout << "Received Finish Sync from Client" << std::endl;
	state = ClientState::Play;
}

void RemoteClient::handlePlayerSync(NetworkPacket& packet)
{
	std::string name;

	glm::vec3 position;
	glm::vec3 rotation;

	packet >> name >> position >> rotation;

	auto server = app->getCurrentServer();

	auto entity = server->findEntity(name);
	if (entity)
	{
		auto transformComp = entity->findComponent<TransformComponent>();
		transformComp->setPosition(position);
		transformComp->setPitch(rotation.x);
		transformComp->setYaw(rotation.y);
		transformComp->setRoll(rotation.z);
	}
}

void RemoteClient::handleMessage(NetworkPacket& packet)
{
	switch (packet.getPacketId())
	{
	case PacketId::CFinishInitialEntitySync:
		if (state == ClientState::Sync)
		{
			handleFinishInitialEntitySync(packet);
		}
		break;

	case PacketId::CPlayerSync:
		if (state == ClientState::Play)
		{
			handlePlayerSync(packet);
		}
		break;

	default:
		std::cout << "Client is out of sync" << std::endl;
		throw std::exception();
	}
}
