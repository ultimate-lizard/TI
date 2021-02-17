#pragma once

#include <TI/Client/Client.h>

class RemoteClient : public Client
{
public:
	RemoteClient(Application* app);

	void connect() {};
	void receiveServerConnectionResponse(ServerConnectionResponse response) override;

private:

};
