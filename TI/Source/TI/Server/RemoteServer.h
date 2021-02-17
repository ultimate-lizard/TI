#pragma once

#include <TI/Server/Server.h>
#include <TI/Network/NetworkHandler.h>

class RemoteServer : public Server
{
public:
	RemoteServer(Application* app);

	void connectClient(ClientConnectionRequest info) override;

	void disconnectClient(Client* client) override {};

private:
	NetworkHandler network;
};
