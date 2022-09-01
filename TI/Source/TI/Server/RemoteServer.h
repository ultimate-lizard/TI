#pragma once

#include <SDL_net.h>

#include <TI/Server/LocalServer.h>

class RemoteServer : public LocalServer
{
public:
	RemoteServer(Application* app);
	~RemoteServer();

	virtual bool connectClient(Client* client, const std::string& ip, int port) override;

	UDPsocket socket;
};
