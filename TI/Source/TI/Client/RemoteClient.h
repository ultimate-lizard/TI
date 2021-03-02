#pragma once

#include <thread>
#include <set>

#include <TI/Client/Client.h>
#include <TI/Network/NetworkHandler.h>

class RemoteClient : public Client
{
public:
	RemoteClient(Application* app);

	void connect() override {};
	void shutdown() override;

	void update(float dt) override;

	void setSocket(Socket socket);
	Socket getSocket() const;

private:
	void waitForMessages();
	// void handleMessage(NetworkPacket& packet);

private:
	Socket socket;
	std::thread messageThread;
	bool shuttingDown;
};
