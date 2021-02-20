#include "NetworkHandler.h"

#include <iostream>

#include <TI/Network/Message.h>

NetworkHandler::NetworkHandler()
{

}

NetworkHandler::~NetworkHandler()
{
	//running = false;

	//if (listenThread.joinable())
	//{
	//	listenThread.join();
	//}

	//if (messageThread.joinable())
	//{
	//	messageThread.join();
	//}

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

bool Socket::send(const void* data, int length)
{
	int sentLength = 0;
	sentLength = SDLNet_TCP_Send(socket, data, length);

	return sentLength == length;
}

bool Socket::receive(void* data, int length)
{
	int receivedLength = 0;
	receivedLength = SDLNet_TCP_Recv(socket, data, length);

	return receivedLength == length;
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
