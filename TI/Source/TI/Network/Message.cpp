#include "Message.h"

#include <iostream>
#include <vector>

bool ClientConnectionRequestNetMessage::serialize(Buffer& buffer)
{
	return buffer.writeString(clientName);
}

bool ClientConnectionRequestNetMessage::deserialzie(Buffer& buffer)
{
	return buffer.readString(clientName);
}

bool ClientConnectionResponseNetMessage::serialize(Buffer& buffer)
{
	return buffer.writeString(clientName);
}

bool ClientConnectionResponseNetMessage::deserialzie(Buffer& buffer)
{
	return buffer.readString(clientName);
}

bool EntityInfoNetMessage::serialize(Buffer& buffer)
{
	bool result = false;

	result = buffer.writeString(name);
	result = buffer.writeString(id);
	result = buffer.writeFloat(x);
	result = buffer.writeFloat(y);
	result = buffer.writeFloat(z);
	result = buffer.writeFloat(pitch);
	result = buffer.writeFloat(yaw);
	result = buffer.writeFloat(roll);

	return result;
}

bool EntityInfoNetMessage::deserialzie(Buffer& buffer)
{
	bool result = false;

	result = buffer.readString(name);
	result = buffer.readString(id);
	result = buffer.readFloat(x);
	result = buffer.readFloat(y);
	result = buffer.readFloat(z);
	result = buffer.readFloat(pitch);
	result = buffer.readFloat(yaw);
	result = buffer.readFloat(roll);

	return result;
}

void serializeNetMessage(NetMessageVariantsType src, Buffer& dest)
{
	if (auto msg = std::get_if<ClientConnectionRequestNetMessage>(&src))
	{
		dest.writeInt(NetMessageType::ClientConnectionRequest);
		msg->serialize(dest);
	}
	else if (auto msg = std::get_if<EntityInfoNetMessage>(&src))
	{
		dest.writeInt(NetMessageType::EntityInfo);
		msg->serialize(dest);
	}
	else if (auto msg = std::get_if<ClientConnectionResponseNetMessage>(&src))
	{
		dest.writeInt(NetMessageType::ClientConnectionResponse);
		msg->serialize(dest);
	}
	else if (auto msg = std::get_if<FinishInitialEntitySyncNetMessage>(&src))
	{
		dest.writeInt(NetMessageType::FinishInitialEntitySync);
	}
	else if (auto msg = std::get_if<ClientReadyNetMessage>(&src))
	{
		dest.writeInt(NetMessageType::ClientReady);
	}
	else
	{
		throw std::exception();
	}
}

NetMessageVariantsType deserializeNetMessage(Buffer& src)
{
	int type = 0;
	src.readInt(type);

	switch (type)
	{
		case NetMessageType::ClientConnectionRequest:
		{
			ClientConnectionRequestNetMessage msg;
			msg.deserialzie(src);
			return msg;
		}

		case NetMessageType::ClientConnectionResponse:
		{
			ClientConnectionResponseNetMessage msg;
			msg.deserialzie(src);
			return msg;
		}

		case NetMessageType::EntityInfo:
		{
			EntityInfoNetMessage msg;
			msg.deserialzie(src);
			return msg;
		}

		case NetMessageType::FinishInitialEntitySync:
			return FinishInitialEntitySyncNetMessage();

		case NetMessageType::ClientReady:
			return ClientReadyNetMessage();

		default:
			std::cout << "Unknown message has been received" << std::endl;
	}

	return {};
}
