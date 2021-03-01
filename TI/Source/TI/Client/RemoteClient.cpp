#include "RemoteClient.h"

#include <iostream>

RemoteClient::RemoteClient(Application* app) :
	Client(app),
	shuttingDown(false)
{
	state = ClientState::Handshake;
	messageThread = std::thread(&RemoteClient::waitForMessages, this);
}

void RemoteClient::shutdown()
{
	socket.close();
	if (messageThread.joinable())
	{
		messageThread.join();
	}

	Client::shutdown();
}

void RemoteClient::update(float dt)
{
	if (shuttingDown)
	{
		shutdown();
	}
}

void RemoteClient::setSocket(Socket socket)
{
	this->socket = socket;
}

void RemoteClient::waitForMessages()
{
	state = ClientState::Sync;

	while (!shuttingDown)
	{
		if (socket)
		{
			try
			{
				NetworkPacket packet;
				socket.receive(packet);
			}
			catch (std::exception&)
			{
				std::cout << "Exception during RemoteClient::waitForMessages" << std::endl;
				socket.close();
				shuttingDown = true;
				break;
			}
		}
	}
}
