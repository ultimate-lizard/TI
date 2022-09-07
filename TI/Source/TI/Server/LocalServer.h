#pragma once

#include <TI/Server/Server.h>

class Star;
class Plane;

class LocalServer : public Server
{
public:
	LocalServer(Application* app);
	~LocalServer();

	void update(float dt) override;

	bool connectClient(Client* const client, const std::string& ip, int port) override;

	void ejectClient(Client* client) override;

protected:
	void initStarSystems();

	void spawnPlayer(Client* const client, Plane* plane, const glm::vec3& position);
};
