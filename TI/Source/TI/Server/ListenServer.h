#pragma once

#include <TI/Server/LocalServer.h>
#include <TI/Network/NetworkHandler.h>

class ListenServer : public LocalServer
{
public:
	ListenServer(Application* app);
	~ListenServer();

	void admitClient(Client*) override;

	void onMessageReceive(const void* data, const int& size);

	void shutdown() override;

	void update(float dt) override;

private:
	void openConnection();
	void waitConnections();

	void handleClientConnectionRequest(Socket socket);

	// void handleConnectionRequestMessage(ClientConnectionRequestMessage message);
	//void acceptConnections(Socket socket);
	//void waitForMessage(Socket socket);
	//void sendEntityInitialInfo(Socket socket);
	//void syncEntities(Socket socket);

private:
	std::thread waitConnectionsThread;
	std::thread waitForMessageThread;
	std::thread syncEntitiesThread;

	NetworkHandler network;

	bool shuttingDown;

	Socket server;

	ServerState state;
};
