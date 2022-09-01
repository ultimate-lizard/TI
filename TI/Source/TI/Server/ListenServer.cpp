#include "ListenServer.h"

#include <istream>

#include <TI/Application.h>

ListenServer::ListenServer(Application* app) :
	LocalServer(app),
	socket(nullptr)
{
	if (SDLNet_Init())
	{
		std::cout << "There was an error initializing SDL_Net: " << SDLNet_GetError() << std::endl;
	}

	if (app)
	{
		app->threadPool.pushTask(std::bind(&ListenServer::listen, this));
	}
}

ListenServer::~ListenServer()
{
}

bool ListenServer::connectClient(Client* client, const std::string& ip, int port)
{
	return false;
}

void ListenServer::ejectClient(Client* client)
{

}

void ListenServer::listen()
{
	socket = SDLNet_UDP_Open(25565);

	SDLNet_SocketSet socketSet;
	socketSet = SDLNet_AllocSocketSet(1);
	SDLNet_UDP_AddSocket(socketSet, socket);

	UDPpacket* packet = SDLNet_AllocPacket(512);

	while (!shuttingDown)
	{
		SDLNet_CheckSockets(socketSet, 33);
		if (SDLNet_SocketReady(socket))
		{
			SDLNet_UDP_Recv(socket, packet);
			receivePacket(packet);
		}
	}

	std::cout << "Stopped listening" << std::endl;
}

void ListenServer::receivePacket(UDPpacket* packet)
{
	if (!packet)
	{
		return;
	}

	size_t size = packet->len;

}

void ListenServer::handleClientConnectionRequest(ClientConnectionRequest msg)
{

}
