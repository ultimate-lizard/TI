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
	openConnection();
}

ListenServer::~ListenServer()
{
	shuttingDown = true;

	if (waitConnectionsThread.joinable())
	{
		waitConnectionsThread.join();
	}

	if (waitForMessageThread.joinable())
	{
		waitForMessageThread.join();
	}
}

void ListenServer::admitClient(Client* client)
{
	createPlayerEntity(client->getName());
	possesEntity(client->getName(), client);
}

void ListenServer::shutdown()
{
	shuttingDown = true;
}

void ListenServer::update(float dt)
{
	LocalServer::update(dt);

	for (auto& client : remoteClients)
	{
		if (client->getState() == ClientState::Play)
		{
			for (auto& mapPair : spawnedEntities)
			{
				auto& entity = mapPair.second;
				std::string id = entity->getId();

				if (id == client->getName())
				{
					continue;
				}

				auto transformComp = entity->findComponent<TransformComponent>();
				if (transformComp)
				{
					glm::vec3 position = transformComp->getPosition();

					glm::vec3 rotation;
					rotation.x = transformComp->getPitch();
					rotation.y = transformComp->getYaw();
					rotation.z = transformComp->getRoll();

					NetworkPacket packet;
					packet.setPacketId(PacketId::SEntitySync);
					packet << id << position << rotation;

					try
					{
						client->getSocket().send(packet);
					}
					catch (std::exception&)
					{
						client->shutdown();
					}
				}
			}
		}
	}
}

void ListenServer::openConnection()
{
	server = network.openConnection(25565);
	if (server)
	{
		waitConnectionsThread = std::thread(&ListenServer::waitConnections, this);
	}
}

void ListenServer::waitConnections()
{
	std::cout << "Started waiting for connections" << std::endl;
	while (!shuttingDown)
	{
		Socket client = server.acceptConnection();
		if (client)
		{
			handleConnectionRequest(client);
		}
	}
}

void ListenServer::handleConnectionRequest(Socket socket)
{
	NetworkPacket request;
	socket.receive(request);
	std::string clientName;
	request >> clientName;

	if (app->findClient(clientName))
	{
		std::cout << "A player with the same name is already connected to the server" << std::endl;
		socket.close();
		return;
	}

	auto remoteClient = std::make_unique<RemoteClient>(app);
	remoteClient->setSocket(socket);
	remoteClient->setName(clientName);
	admitClient(remoteClient.get());

	try
	{
		// Send response
		NetworkPacket response;
		response.setPacketId(PacketId::SConnectionResponse);
		socket.send(response);

		// Initial sync entities
		sendEntityInitialSync(remoteClient.get());
	}
	catch (std::exception&)
	{
		std::cout << "Exception during ListenServer::handleConnectionRequest" << std::endl;
		socket.close();
	}

	remoteClients.push_back(remoteClient.get());
	app->addClient(std::move(remoteClient));
}

void ListenServer::sendEntityInitialSync(RemoteClient* client)
{
	for (auto& mapPair : spawnedEntities)
	{
		auto& entity = mapPair.second;

		if (entity->getId() == client->getName())
		{
			continue;
		}

		NetworkPacket packet;
		packet.setPacketId(PacketId::SEntityInitialSync);

		packet << entity->getName() << entity->getId();

		glm::vec3 position;
		glm::vec3 rotation;

		auto transformComp = entity->findComponent<TransformComponent>();
		if (transformComp)
		{
			position = transformComp->getPosition();
			rotation.x = transformComp->getPitch();
			rotation.y = transformComp->getYaw();
			rotation.z = transformComp->getRoll();
		}

		packet << position << rotation;

		try
		{
			client->getSocket().send(packet);
		}
		catch (std::exception&)
		{
			std::cout << "Exception during sending entity initial info" << std::endl;
			ejectClient(client);
			client->shutdown();
		}
	}

	NetworkPacket finishPacket;
	finishPacket.setPacketId(PacketId::SFinishInitialEntitySync);
	client->getSocket().send(finishPacket);
}
