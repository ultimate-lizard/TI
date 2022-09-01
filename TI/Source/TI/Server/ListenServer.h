#pragma once

#include <SDL_net.h>

#include <TI/Server/LocalServer.h>
#include <TI/Network/NetworkProtocol.h>

class ListenServer : public LocalServer
{
public:
	ListenServer(class Application* app);
	~ListenServer();

	bool connectClient(Client* client, const std::string& ip, int port) override;
	void ejectClient(Client* client) override;

private:
	void listen();
	void receivePacket(UDPpacket* packet);

	void handleClientConnectionRequest(ClientConnectionRequest msg);

	UDPsocket socket;
};