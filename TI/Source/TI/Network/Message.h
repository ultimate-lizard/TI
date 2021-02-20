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

class EntityInfoMessage
{
public:
	std::string name;
	std::string id;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

	bool serialize(void* data);
	bool deserialzie(const void* data);
};