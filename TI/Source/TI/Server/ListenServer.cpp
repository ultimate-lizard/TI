#include "ListenServer.h"

#include <iostream>

#include <SDL_net.h>

#include <TI/Client/RemoteClient.h>
#include <TI/Application.h>
#include <TI/Server/Component/TransformComponent.h>

ListenServer::ListenServer(Application* app) :
	LocalServer(app),
	shuttingDown(false)
{
	server = network.openConnection(25565);
	if (server)
	{
		acceptConnectionsThread = std::thread(&ListenServer::acceptConnections, this, server);
	}
}

ListenServer::~ListenServer()
{
	shuttingDown = true;

	if (acceptConnectionsThread.joinable())
	{
		acceptConnectionsThread.join();
	}

	if (waitForMessageThread.joinable())
	{
		waitForMessageThread.join();
	}
}

void ListenServer::connectClient(ClientConnectionRequest request)
{
	connectedClients.emplace(request.client->getName(), request.client);

	createPlayerEntity(request.client->getName());
	possesEntity(request.client->getName(), request.client);
}

void ListenServer::onMessageReceive(const void* data, const int& size)
{
	ClientConnectionRequestMessage msg;
	msg.deserialzie(data);

	handleConnectionRequestMessage(msg);
}

void ListenServer::shutdown()
{
	shuttingDown = true;
}

void ListenServer::update(float dt)
{
	LocalServer::update(dt);

	for (auto& mapPair : spawnedEntities)
	{
		auto& entity = mapPair.second;
		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			// transformComp->get
		}
	}
}

void ListenServer::handleConnectionRequestMessage(ClientConnectionRequestMessage message)
{
	if (app)
	{
		auto client = std::make_unique<RemoteClient>(app);
		client->setName(message.clientName);
		auto clientPtr = client.get();
		app->addClient(std::move(client));

		ClientConnectionRequest connectionRequest;
		connectionRequest.client = clientPtr;
		connectClient(std::move(connectionRequest));
	}
}

void ListenServer::acceptConnections(Socket socket)
{
	std::cout << "Waiting for client..." << std::endl;
	while (!shuttingDown)
	{
		Socket client = server.acceptConnection();
		if (client)
		{
			handleNewConnection(client);
		}
	}
}

void ListenServer::handleNewConnection(Socket socket)
{
	char data[512];
	if (socket.receive(data, 512))
	{
		ClientConnectionRequestMessage msg;
		msg.deserialzie(data);

		if (app)
		{
			auto client = std::make_unique<RemoteClient>(app);
			client->setName(msg.clientName);

			Client* clientPtr = client.get();
			app->addClient(std::move(client));

			ClientConnectionRequest request;
			request.client = clientPtr;

			connectClient(std::move(request));

			socket.setName(msg.clientName);

			sendEntityInitialInfo(socket);
		}

		waitForMessageThread = std::thread(&ListenServer::waitForMessage, this, socket);
	}
}

void ListenServer::waitForMessage(Socket socket)
{
	while (!shuttingDown)
	{
		char data[512];
		if (!socket.receive(data, 512))
		{
			std::cout << "The client has disconnected" << std::endl;

			app->removeClient(socket.getName());

			break;
		}
	}
}

void ListenServer::sendEntityInitialInfo(Socket socket)
{
	for (auto& mapPair : spawnedEntities)
	{
		auto& entity = mapPair.second;

		EntityInfoMessage infoMsg;
		infoMsg.name = entity->getName();
		infoMsg.id = entity->getId();

		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			// TODO: Implement packet builder
			auto pos = transformComp->getPosition();
			infoMsg.x = pos.x;
			infoMsg.y = pos.y;
			infoMsg.z = pos.z;
			infoMsg.pitch = transformComp->getPitch();
			infoMsg.yaw = transformComp->getYaw();
			infoMsg.roll = transformComp->getRoll();
		}

		char data[512];
		infoMsg.serialize(data);

		socket.send(data, 512);
	}
}
