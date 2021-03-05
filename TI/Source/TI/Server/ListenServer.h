#pragma once

#include <TI/Server/LocalServer.h>
#include <TI/Network/NetworkHandler.h>

class RemoteClient;

class ListenServer : public LocalServer
{
public:
	ListenServer(Application* app);
	~ListenServer();

	void admitClient(Client*) override;

	void shutdown() override;

	void update(float dt) override;

private:
	void openConnection();
	void waitConnections();

	void handleConnectionRequest(Socket socket);

	void sendEntityInitialSync(RemoteClient* client);

private:
	std::thread waitConnectionsThread;
	std::thread waitForMessageThread;
	std::thread syncEntitiesThread;

	NetworkHandler network;

	bool shuttingDown;

	Socket server;

	std::vector<RemoteClient*> remoteClients;
};
