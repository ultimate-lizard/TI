#include "RemoteServer.h"

#include <TI/Network/NetworkProtocol.h>

RemoteServer::RemoteServer(Application* app) :
	LocalServer(app)
{
	if (SDLNet_Init())
	{
		std::cout << "There was an error initializing SDL_Net: " << SDLNet_GetError() << std::endl;
	}

	socket = SDLNet_UDP_Open(0);
}

RemoteServer::~RemoteServer()
{
	SDLNet_Quit();
}

bool RemoteServer::connectClient(Client* client, const std::string& ip, int port)
{
	IPaddress ipAddr;
	SDLNet_ResolveHost(&ipAddr, ip.c_str(), port);
	SDLNet_UDP_Bind(socket, -1, &ipAddr);

	UDPpacket* packet = SDLNet_AllocPacket(512);
	packet->address = ipAddr;

	

	return false;
}
