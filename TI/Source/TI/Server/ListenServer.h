#pragma once

#include <TI/Server/LocalServer.h>
#include <TI/Network/NetworkHandler.h>
#include <TI/Network/Message.h>



class ListenServer : public LocalServer
{
public:
	ListenServer(Application* app);
	~ListenServer();

	void connectClient(Client*) override;

	void onMessageReceive(const void* data, const int& size);

	void shutdown() override;

	void update(float dt) override;

private:
	// void handleConnectionRequestMessage(ClientConnectionRequestMessage message);
	void acceptConnections(Socket socket);
	void handleNewConnection(Socket socket);
	void waitForMessage(Socket socket);
	void sendEntityInitialInfo(Socket socket);
	void syncEntities(Socket socket);

private:
	std::thread acceptConnectionsThread;
	std::thread waitForMessageThread;
	std::thread syncEntitiesThread;

	NetworkHandler network;

	bool shuttingDown;

	Socket server;

	ServerState state;
};
