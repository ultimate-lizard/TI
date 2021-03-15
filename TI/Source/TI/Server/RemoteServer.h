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

	void connectClient(Client* client, const std::string& ip, int port) override;
	void ejectClient(Client* client) override {};

	void update(float dt) override;

	void shutdown() override;

private:
	void waitForMessages();
	void handleMessage(NetworkPacket& packet);

	void handleInitialEntitySync(NetworkPacket& packet);
	void handleFinishInitialEntitySync(NetworkPacket& packet);

	void handleEntitySync(NetworkPacket& packet);
	void handleSpawnPlayerEntity(NetworkPacket& packet);
	void handleDestroyEntity(NetworkPacket& packet);

	void sendPlayerInfo(Client* client);

private:
	NetworkHandler network;

	std::thread waitForMessageThread;

	Socket server;

	RemoteServerState state;
};
