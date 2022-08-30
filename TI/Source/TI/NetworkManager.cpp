#include "NetworkManager.h"

#include <iostream>

#include <SDL_net.h>

#include <TI/Server/RemoteServer.h>

void NetworkManager::waitForRemoteClients()
{
	UDPpacket* packet = SDLNet_AllocPacket(512);
	while (SDLNet_UDP_Recv(RemoteServer::socket, packet) == 0)
	{

	}

	std::cout << "RECEIVED SOMETHING" << std::endl;
}
