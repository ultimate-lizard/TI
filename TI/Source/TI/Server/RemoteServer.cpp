#include "RemoteServer.h"

#include <iostream>

#include <TI/Network/Message.h>

RemoteServer::RemoteServer(Application* app) :
	Server(app)
{
	// network.connect("localhost")
}

void RemoteServer::connectClient(ClientConnectionRequest info)
{
	Socket server = network.connect("localhost", 25565);
	if (server)
	{
		ClientConnectionRequestMessage msg;
		msg.clientId = 123;
		msg.clientName = "RemotePlayer1";

		char data[512];
		msg.serialize(data);

		if (!server.send(data, 512))
		{
			std::cout << "Unable to send the message to server" << std::endl;
		}
	}


	//if (network.connect("localhost", 25565))
	//{
	//	ClientConnectionRequestMessage msg;
	//	msg.clientName = "RemotePlayer";

	//	char buf[512];
	//	msg.serialize(buf);
	//	//network.send(buf, 512);

	//	// Wait for response
	//	// network.receive();

	//	// If ok:
	//	/*connectedClients.emplace(request.client->getName(), request.client);
	//	createPlayerEntity(request.client->getName());
	//	possesEntity(request.client->getName(), request.client);*/
	//}
}
