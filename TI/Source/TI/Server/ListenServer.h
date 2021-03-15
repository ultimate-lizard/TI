#pragma once

#include <TI/Server/LocalServer.h>
#include <TI/Network/NetworkHandler.h>

class RemoteClient;

class ListenServer : public LocalServer
{
public:
	ListenServer(Application* app, int port);
	~ListenServer();

	void connectClient(Client* client, const std::string& ip, int port) override;
	void ejectClient(Client* client) override;

	void shutdown() override;

	void update(float dt) override;

private:
	void openConnection(int port);
	void waitConnections();

	void broadcastEntitiesInfo();
	void broadcastPlayerEntitySpawn(Entity* entity);
	void broadcastDestroyEntity(Entity* entity);

	void handleConnectionRequest(Socket socket);

	void sendEntityInitialSync(RemoteClient* client);

private:
	std::thread waitConnectionsThread;
	std::thread waitForMessageThread;
	
	NetworkHandler network;

	bool shuttingDown;

	Socket server;

	std::vector<RemoteClient*> remoteClients;
};
