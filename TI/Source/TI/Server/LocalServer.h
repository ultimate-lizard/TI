#pragma once

#include <TI/Server/Server.h>

class LocalServer : public Server
{
public:
	LocalServer(Application* app);

	void update(float dt) override;

	void connectClient(ClientConnectionRequest request) override;

	void disconnectClient(Client* client) override;

protected:
	void createCubes();
	void createPlayerEntity(const std::string& name);
	void possesEntity(const std::string& entityName, Client* client);
};
