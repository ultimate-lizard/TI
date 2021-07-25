#include "ListenServer.h"

#include <iostream>

#include <SDL_net.h>

#include <TI/Client/RemoteClient.h>
#include <TI/Application.h>

ListenServer::ListenServer(Application* app, int port) :
	LocalServer(app),
	shuttingDown(false)
{
	openConnection(port);
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

void ListenServer::connectClient(Client* client, const std::string&, int)
{
	// spawnEntity(client->getName());
	possesEntity(client->getName(), client);

	broadcastPlayerEntitySpawn(findEntity(client->getName()));
}

void ListenServer::ejectClient(Client* client)
{
	auto iter = std::find(remoteClients.begin(), remoteClients.end(), client);
	if (iter != remoteClients.end())
	{
		remoteClients.erase(iter);
	}

	broadcastDestroyEntity(findEntity(client->getName()));

	LocalServer::ejectClient(client);
}

void ListenServer::shutdown()
{
	shuttingDown = true;
}

void ListenServer::update(float dt)
{
	LocalServer::update(dt);

	broadcastEntitiesInfo();

	SDL_Delay(10);
}

void ListenServer::openConnection(int port)
{
	server = network.openConnection(port);
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

void ListenServer::broadcastEntitiesInfo()
{
	for (auto& client : remoteClients)
	{
		if (client->getState() == ClientState::Play)
		{
			for (auto& mapPair : spawnedEntities)
			{
				auto& entity = mapPair.second;
				std::string id = entity->getId();

				// Don't send info about the player entity to the remote entity owner
				if (id == client->getName())
				{
					continue;
				}

				glm::vec3 position = entity->getPosition();

				glm::vec3 rotation;
				/*rotation.x = transformComp->getPitch();
				rotation.y = transformComp->getYaw();
				rotation.z = transformComp->getRoll();*/

				NetworkPacket packet;
				packet.setPacketId(PacketId::SEntitySync);
				packet << id << position << rotation;

				try
				{
					client->getSocket().send(packet);
				}
				catch (std::exception&)
				{
					std::cout << "Lost connection to the remote client or an error occurred during package receipt" << std::endl;
					client->shutdown();
				}
			}
		}
	}
}

void ListenServer::broadcastPlayerEntitySpawn(Entity* entity)
{
	if (!entity)
	{
		return;
	}

	for (auto& client : remoteClients)
	{
		if (client->getState() == ClientState::Play)
		{
			NetworkPacket packet;
			packet.setPacketId(PacketId::SSpawnPlayerEntity);

			glm::vec3 rotation;
			/*rotation.x = transformComp->getPitch();
			rotation.y = transformComp->getYaw();
			rotation.z = transformComp->getRoll();*/

			packet << entity->getId() << entity->getPosition() << rotation;

			// Broadcast only to NOT owners of the entity
			if (client->getName() != entity->getId())
			{
				client->getSocket().send(packet);
			}
		}
	}
}

void ListenServer::broadcastDestroyEntity(Entity* entity)
{
	if (!entity)
	{
		return;
	}

	NetworkPacket packet;
	packet.setPacketId(PacketId::SDestroyEntity);

	packet << entity->getId();

	for (auto& client : remoteClients)
	{
		try
		{
			client->getSocket().send(packet);
		}
		catch (std::exception&)
		{
			client->getSocket().close();
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
		return;
	}

	remoteClients.push_back(remoteClient.get());

	app->addClient(std::move(remoteClient));

	connectClient(app->findClient(clientName), "", 0);
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

		position = entity->getPosition();
		rotation = entity->getRotation();

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
