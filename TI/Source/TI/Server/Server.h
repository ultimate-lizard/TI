#pragma once

#include <map>

#include <TI/Client/ClientConnectionRequest.h>

class Application;

class Server
{
public:
	Server(Application* app) : app(app) {}
	virtual ~Server() = default;

	virtual void update(float dt) {};

	virtual void connectClient(ClientConnectionRequest info) = 0;

	virtual void shutdown() {}

protected:
	Application* app;

	std::map<int, Client*> connectedClients;
};

class LocalServer : public Server
{
public:
	LocalServer(Application* app) : Server(app) {}

	void connectClient(ClientConnectionRequest request) override;

private:
	
};
