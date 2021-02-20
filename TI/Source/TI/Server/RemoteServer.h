#pragma once

#include <thread>

#include <TI/Server/Server.h>
#include <TI/Network/NetworkHandler.h>

// TODO: Derive from LocalServer
class RemoteServer : public Server
{
public:
	RemoteServer(Application* app);
	~RemoteServer();

	void connectClient(ClientConnectionRequest info) override;

	void disconnectClient(Client* client) override {};

	void update(float dt) override;

	void waitForMessage(Socket socket);

private:
	NetworkHandler network;

	std::thread waitForMessageThread;

	bool shuttingDown;
};
