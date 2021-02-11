#include "Server.h"

#include <TI/Application.h>
#include <TI/Client/Client.h>

static int clientId = 0;

void LocalServer::connectClient(ClientConnectionRequest request)
{
	// Send packet with client info to IP
	// If IP responds with ConnectionSuccessful packet:
	// client->receiveServerConnectionResponse(ServerConnectionInfo info);
	request.client->receiveServerConnectionResponse({ ++clientId });
	connectedClients.emplace(clientId, request.client);
}
