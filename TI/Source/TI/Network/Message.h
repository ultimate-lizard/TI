#pragma once

#include <string>
#include <iostream>

class ClientConnectionRequestMessage
{
public:
	int clientId;
	std::string clientName;

	bool serialize(void* data);
	bool deserialzie(const void* data);
};
