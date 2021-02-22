#include "NetworkHandler.h"

#include <iostream>

#include <TI/Network/Message.h>

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

bool Socket::send(Buffer& buffer, int size)
{
	char b[512];
	memcpy(b, buffer.getBuffer(), size);

	int sentLength = 0;
	sentLength = SDLNet_TCP_Send(socket, buffer.getBuffer(), size);

	return sentLength == size;
}

bool Socket::receive(Buffer& buffer, int size)
{
	char rawBuf[512];

	int receivedLength = 0;
	receivedLength = SDLNet_TCP_Recv(socket, rawBuf, size);

	buffer.reset(rawBuf, size);

	return receivedLength == size;
}

void Socket::setName(const std::string& name)
{
	this->name = name;
}

const std::string& Socket::getName() const
{
	return name;
}

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
	}
}
