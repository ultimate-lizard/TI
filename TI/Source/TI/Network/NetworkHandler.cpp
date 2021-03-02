#include "NetworkHandler.h"

#include <iostream>

#include <TI/Util/Serialization.h>

NetworkHandler::NetworkHandler()
{

}

NetworkHandler::~NetworkHandler()
{
	SDLNet_Quit();
}

Socket NetworkHandler::openConnection(int port)
{
	if (SDLNet_Init())
	{
		std::cout << "Can't initialize SDL_Net" << std::endl;
		return nullptr;
	}

	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, NULL, port) == -1)
	{
		std::cout << "Couldn't resolve" << std::endl;
		return nullptr;
	}

	TCPsocket socket = SDLNet_TCP_Open(&ip);
	if (!socket)
	{
		std::cout << "Couldn't open the connection: " << SDLNet_GetError() << std::endl;
		return nullptr;
	}

	return Socket(socket);
}

void NetworkHandler::closeConnection(Socket socket)
{
	if (socket.socket)
	{
		SDLNet_TCP_Close(socket.socket);
	}
}

Socket NetworkHandler::connect(const std::string& ip, int port)
{
	if (SDLNet_Init())
	{
		std::cout << "Can't initialize SDL_Net" << std::endl;
	}

	IPaddress resolvedIp;
	if (SDLNet_ResolveHost(&resolvedIp, ip.c_str(), port) == -1)
	{
		std::cout << "Couldn't resolve IP" << std::endl;
		return Socket();
	}
	
	TCPsocket socket = SDLNet_TCP_Open(&resolvedIp);
	return Socket(socket);
}

//bool Socket::send(NetworkPacket& packet)
//{
//	char b[512];
//	memcpy(b, buffer.getBuffer(), size);
//
//	int sentLength = 0;
//	sentLength = SDLNet_TCP_Send(socket, buffer.getBuffer(), size);
//
//	return sentLength == size;
//}

void Socket::send(NetworkPacket& packet)
{
	int packetSize = static_cast<int>(packet.getSize());
	int sentSize = SDLNet_TCP_Send(socket, &packetSize, sizeof(int));
	if (sentSize != sizeof(int))
	{
		std::cout << "Unable to send packet size" << std::endl;
		throw std::exception();
	}

	int packetId = static_cast<int>(packet.getPacketId());
	sentSize = 0;
	sentSize = SDLNet_TCP_Send(socket, &packetId, sizeof(int));
	if (sentSize != sizeof(int))
	{
		std::cout << "Unable to send packet ID" << std::endl;
		throw std::exception();
	}

	sentSize = 0;
	if (packetSize > 0)
	{
		sentSize = SDLNet_TCP_Send(socket, packet.getData(), packetSize);
		if (sentSize != packet.getSize())
		{
			std::cout << "Unable to send packet" << std::endl;
			throw std::exception();
		}
	}
}

void Socket::receive(NetworkPacket& packet)
{
	int packetSize = 0;
	int receivedSize = 0;

	receivedSize = SDLNet_TCP_Recv(socket, &packetSize, sizeof(int));
	if (receivedSize != sizeof(int))
	{
		std::cout << "Unable to receive packet size" << std::endl;
		throw std::exception();
	}

	int packetId = 0;
	receivedSize = 0;
	receivedSize = SDLNet_TCP_Recv(socket, &packetId, sizeof(int));
	if (receivedSize != sizeof(int))
	{
		std::cout << "Unable to receive packet ID" << std::endl;
		throw std::exception();
	}

	std::vector<char> buffer;
	buffer.resize(packetSize);

	if (packetSize > 0)
	{
		receivedSize = 0;
		receivedSize = SDLNet_TCP_Recv(socket, &buffer[0], packetSize);
		if (receivedSize != packetSize)
		{
			std::cout << "Unable to receive packet" << std::endl;
			throw std::exception();
		}

		packet.reset(&buffer[0], packetSize, 0);
	}

	packet.setPacketId(static_cast<PacketId>(packetId));
}

//bool Socket::receive(NetworkPacket& packet)
//{
//	char rawBuf[512];
//
//	int receivedLength = 0;
//	receivedLength = SDLNet_TCP_Recv(socket, rawBuf, size);
//
//	buffer.reset(rawBuf, size);
//
//	return receivedLength == size;
//}

Socket Socket::acceptConnection()
{
	TCPsocket client = SDLNet_TCP_Accept(socket);

	return Socket(client);
}

void Socket::close()
{
	if (socket)
	{
		SDLNet_TCP_Close(socket);
		socket = nullptr;
	}
}
