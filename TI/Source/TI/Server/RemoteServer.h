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

	void connectClient(Client* client) override;

	void disconnectClient(Client* client) override {};

	void update(float dt) override;

	void waitForMessage();

private:
	NetworkHandler network;

	std::thread waitForMessageThread;

	bool shuttingDown;

	Socket server;

	ServerState state;
};
