#pragma once

#include <TI/Server/Server.h>

class LocalServer : public Server
{
public:
	LocalServer(Application* app);

	void update(float dt) override;

	void connectClient(Client* const client, const std::string& ip, int port) override;

	void ejectClient(Client* client) override;

protected:
	void spawnPlayer(Client* const client);
};
