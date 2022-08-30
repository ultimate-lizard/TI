#include "RemoteServer.h"

UDPsocket RemoteServer::socket;

RemoteServer::RemoteServer(Application* app) :
	LocalServer(app)
{
	if (SDLNet_Init())
	{
		std::cout << "There was an error initializing SDL_Net: " << SDLNet_GetError() << std::endl;
	}

	socket = SDLNet_UDP_Open(25565);
	SDL_Delay(1);
}

RemoteServer::~RemoteServer()
{
	SDLNet_Quit();
}

bool RemoteServer::connectClient(Client* client, const std::string& ip, int port)
{
	UDPpacket* pack = SDLNet_AllocPacket(512);
	SDL_Delay(4000);
	int res = SDLNet_UDP_Send(socket, -1, pack);

	return false;
}
