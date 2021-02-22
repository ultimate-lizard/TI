#pragma once

#include <variant>

#include <TI/Network/ByteBuffer.h>

class ClientConnectionRequestNetMessage
{
public:
	std::string clientName;

	bool serialize(Buffer& buffer);
	bool deserialzie(Buffer& buffer);
};

class EntityInfoNetMessage
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

class ClientConnectionResponseNetMessage
{
public:
	std::string clientName;

	bool serialize(Buffer& buffer);
	bool deserialzie(Buffer& buffer);
};

class FinishInitialEntitySyncNetMessage
{

};

class ClientReadyNetMessage
{

};

using NetMessageVariantsType = std::variant<
	ClientConnectionRequestNetMessage,
	ClientConnectionResponseNetMessage,
	EntityInfoNetMessage,
	FinishInitialEntitySyncNetMessage,
	ClientReadyNetMessage
>;

enum NetMessageType
{
	ClientConnectionRequest,
	ClientConnectionResponse,
	EntityInfo,
	FinishInitialEntitySync,
	ClientReady
};

void serializeNetMessage(NetMessageVariantsType src, Buffer& dest);
NetMessageVariantsType deserializeNetMessage(Buffer& src);
