#include "Message.h"

#include <vector>

bool ClientConnectionRequestMessage::serialize(Buffer& buffer)
{
	return buffer.writeString(clientName);
}

bool ClientConnectionRequestMessage::deserialzie(Buffer& buffer)
{
	return buffer.readString(clientName);
}

bool EntityInfoMessage::serialize(Buffer& buffer)
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

bool EntityInfoMessage::deserialzie(Buffer& buffer)
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

std::variant<ClientConnectionRequestMessage, EntityInfoMessage> deserializeNetMsg(Buffer& buffer)
{
	int msgType;

	if (!buffer.readInt(msgType))
	{
		throw std::exception();
	}
	
	switch (msgType)
	{
	case MSG_CLIENT_CONNECTION_REQUEST:
		{
			ClientConnectionRequestMessage msg;
			if (msg.deserialzie(buffer))
			{
				return msg;
			}
			else
			{
				throw std::exception();
			}
		}
		break;

	case MSG_ENTITY_INFO:
		{
			EntityInfoMessage msg;
			if (msg.deserialzie(buffer))
			{
				return msg;
			}
			else
			{
				throw std::exception();
			}
		}
		break;

	//default:
		//throw std::exception();
	}
}

void serializeNetMsg(std::variant<ClientConnectionRequestMessage, EntityInfoMessage> msg, Buffer& buffer)
{
	bool result = false;

	if (auto msgPtr = std::get_if<ClientConnectionRequestMessage>(&msg); msgPtr)
	{
		result = buffer.writeInt(MSG_CLIENT_CONNECTION_REQUEST);
		result = msgPtr->serialize(buffer);
	}
	else if (auto msgPtr = std::get_if<EntityInfoMessage>(&msg); msgPtr)
	{
		result = buffer.writeInt(MSG_ENTITY_INFO);
		result = msgPtr->serialize(buffer);
	}

	if (!result)
	{
		throw std::exception();
	}
}
