#pragma once

#include <variant>

#include <TI/Network/ByteBuffer.h>

class ClientConnectionRequestMessage
{
public:
	std::string clientName;

	bool serialize(Buffer& buffer);
	bool deserialzie(Buffer& buffer);
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

	bool serialize(Buffer& buffer);
	bool deserialzie(Buffer& buffer);
};

std::variant<ClientConnectionRequestMessage, EntityInfoMessage> deserializeNetMsg(Buffer& buffer);
void serializeNetMsg(std::variant<ClientConnectionRequestMessage, EntityInfoMessage> msg, Buffer& buffer);
