#pragma once

#include <TI/Server/Server.h>

class Star;
// class BlockGrid;
class Planet;

class LocalServer : public Server
{
public:
	LocalServer(Application* app);
	virtual ~LocalServer();

	void update(float dt) override;

	bool connectClient(Client* const client, const std::string& ip, int port) override;

	void ejectClient(Client* client) override;

	CelestialBody* findClosestCelestialBody(CoordinateSystem cs, const TransformComponent& transform) const;

protected:
	void initHomeSolarSystem();
	void initRandomSolarSystem();

	void spawnPlayer(Client* const client, Planet* plane, const glm::vec3& position);
};
