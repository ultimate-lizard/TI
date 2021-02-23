#pragma once

#include <TI/Server/Server.h>

class LocalServer : public Server
{
public:
	LocalServer(Application* app);

	void update(float dt) override;

	void admitClient(Client* client) override;

	void ejectClient(Client* client) override;

protected:
	void createCubes();

};
