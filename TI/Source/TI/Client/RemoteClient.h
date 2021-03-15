#pragma once

#include <thread>
#include <set>
#include <mutex>

#include <TI/Client/Client.h>
#include <TI/Network/NetworkHandler.h>

class RemoteClient : public Client
{
public:
	RemoteClient(Application* app);

	void connect() override {};
	void shutdown() override;

	void setSocket(Socket socket);
	Socket getSocket() const;

private:
	void waitForMessages();
	void handleFinishInitialEntitySync(NetworkPacket& packet);
	void handlePlayerSync(NetworkPacket& packet);
	void handleMessage(NetworkPacket& packet);

private:
	Socket socket;
	std::thread messageThread;
	std::mutex socketMutex;
};
