#pragma once

#include <thread>

#include <TI/Server/Server.h>
#include <TI/Network/NetworkHandler.h>

enum class RemoteServerState
{
	Undefined,
	Handshake,
	Sync,
	Play
};

// TODO: Derive from LocalServer
class RemoteServer : public Server
{
public:
	RemoteServer(Application* app);
	~RemoteServer();

	void admitClient(Client* client) override;
	void ejectClient(Client* client) override {};

	void update(float dt) override;

private:
	void waitForMessages();
	void handleMessage(NetworkPacket& packet);

	void handleInitialEntitySync(NetworkPacket& packet);
	void handleFinishInitialEntitySync(NetworkPacket& packet);

private:
	NetworkHandler network;

	std::thread waitForMessageThread;

	bool shuttingDown;

	Socket server;

	RemoteServerState state;
};
