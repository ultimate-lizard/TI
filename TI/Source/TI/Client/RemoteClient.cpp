#include "RemoteClient.h"

#include <iostream>



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
	while (!shuttingDown)
	{
		if (socket)
		{
			try
			{
				NetworkPacket packet;
				socket.receive(packet);
				// handleMessage(packet);
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
//
//void RemoteClient::handleMessage(NetworkPacket& packet)
//{
//	switch (packet.getPacketId())
//	{
//	case PacketId::SEntityInitialSync:
//		if (state == ClientState::Sync)
//		{
//			handleInitialEntitySync(packet);
//		}
//		break;
//
//	case PacketId::SFinishInitialEntitySync:
//	{
//		if (state == ClientState::Sync)
//		{
//			NetworkPacket packet;
//			packet.setPacketId(PacketId::CFinishInitialEntitySync);
//			socket.send(packet);
//			state = ClientState::Play;
//		}
//	}
//	break;
//
//	case PacketId::SEntitySync:
//		if (state == ClientState::Play)
//		{
//
//		}
//		break;
//
//	default:
//		std::cout << "Client is out of sync" << std::endl;
//		throw std::exception();
//	}
//}
//
//void RemoteClient::handleInitialEntitySync(NetworkPacket& packet)
//{
//	std::string name;
//	std::string id;
//	glm::vec3 position;
//	glm::vec3 rotation;
//
//	packet >> name >> id >> position >> rotation;
//}
//
//void RemoteClient::handleFinishInitialEntitySync(NetworkPacket& packet)
//{
//	state = ClientState::Play;
//
//	NetworkPacket packet;
//	packet.setPacketId(PacketId::CFinishInitialEntitySync);
//	socket.send(packet);
//}
