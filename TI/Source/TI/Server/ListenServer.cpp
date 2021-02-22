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

void ListenServer::connectClient(Client* client)
{
	connectedClients.emplace(client->getName(), client);

	createPlayerEntity(client->getName());
	possesEntity(client->getName(), client);
}

void ListenServer::onMessageReceive(const void* data, const int& size)
{
	// ClientConnectionRequestMessage msg;
	// msg.deserialzie(data);

	// handleConnectionRequestMessage(msg);
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

//void ListenServer::handleConnectionRequestMessage(ClientConnectionRequestMessage message)
//{
//	if (app)
//	{
//		auto client = std::make_unique<RemoteClient>(app);
//		client->setName(message.clientName);
//		auto clientPtr = client.get();
//		app->addClient(std::move(client));
//
//		ClientConnectionRequest connectionRequest;
//		connectionRequest.client = clientPtr;
//		connectClient(std::move(connectionRequest));
//	}
//}

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
	Buffer buffer(512);
	if (socket.receive(buffer, 512))
	{
		// Handle client connection request
		auto msg = deserializeNetMessage(buffer);
		if (auto clientMsg = std::get_if<ClientConnectionRequestNetMessage>(&msg))
		{
			if (app)
			{
				auto client = std::make_unique<RemoteClient>(app);
				client->setName(clientMsg->clientName);

				Client* clientPtr = client.get();
				app->addClient(std::move(client));

				connectClient(clientPtr);

				socket.setName(clientMsg->clientName);

				// Handle client connection response
				ClientConnectionResponseNetMessage response;
				response.clientName = socket.getName();

				buffer.clear();
				serializeNetMessage(response, buffer);

				socket.send(buffer, 512);

				sendEntityInitialInfo(socket);
			}

			waitForMessageThread = std::thread(&ListenServer::waitForMessage, this, socket);
		}
		else
		{
			throw std::exception();
		}
	}
}

void ListenServer::waitForMessage(Socket socket)
{
	while (!shuttingDown)
	{
		Buffer buffer(512);
		if (!socket.receive(buffer, 512))
		{
			// Handle client disconnection
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

		EntityInfoNetMessage infoMsg;
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

		Buffer buff(512);
		serializeNetMessage(infoMsg, buff);

		socket.send(buff, 512);
	}
}
