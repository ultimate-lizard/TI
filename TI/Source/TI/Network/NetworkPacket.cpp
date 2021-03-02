#include "NetworkPacket.h"

#include <TI/Util/Serialization.h>

NetworkPacket::NetworkPacket() :
	id(PacketId::Invalid),
	peerId(0),
	dataSize(0),
	offset(0)
{

}

NetworkPacket::NetworkPacket(PacketId packetId) : 
	id(packetId),
	peerId(0),
	dataSize(0),
	offset(0)
{
}

NetworkPacket::NetworkPacket(const char* data, int size, int peerId) :
	id(PacketId::Invalid),
	peerId(peerId),
	dataSize(0),
	offset(0)
{
	dataSize = size;
	this->data.resize(dataSize);

	if (dataSize > 0)
	{
		memcpy(&this->data[0], data, dataSize);
	}
}

void NetworkPacket::reset(const char* data, int size, int peerId)
{
	this->data.clear();
	this->data.resize(0);

	dataSize = size;
	this->data.resize(dataSize);

	if (dataSize > 0)
	{
		memcpy(&this->data[0], data, dataSize);
	}
}

void NetworkPacket::setPacketId(PacketId id)
{
	this->id = id;
}

PacketId NetworkPacket::getPacketId() const
{
	return id;
}

int NetworkPacket::getPeerId() const
{
	return peerId;
}

size_t NetworkPacket::getSize() const
{
	return dataSize;
}

const char* NetworkPacket::getData() const
{
	return &data[0];
}

NetworkPacket& NetworkPacket::operator<<(int src)
{
	checkWriteBounds(sizeof(int));

	writeInt(&data[offset], src);
	offset += sizeof(int);

	return *this;
}

NetworkPacket& NetworkPacket::operator>>(int& dst)
{
	checkReadBounds(sizeof(int));

	readInt(dst, &data[offset]);
	offset += sizeof(int);

	return *this;
}

NetworkPacket& NetworkPacket::operator<<(float src)
{
	checkWriteBounds(sizeof(float));

	writeFloat(&data[offset], src);
	offset += sizeof(float);

	return *this;
}

NetworkPacket& NetworkPacket::operator>>(float& dst)
{
	checkReadBounds(sizeof(float));

	readFloat(dst, &data[offset]);
	offset += sizeof(float);

	return *this;
}

NetworkPacket& NetworkPacket::operator<<(const std::string& src)
{
	*this << static_cast<int>(src.size() + 1);

	checkWriteBounds(static_cast<int>(src.size() + 1));

	writeString(&data[offset], src);
	offset += src.size() + 1;

	return *this;
}


NetworkPacket& NetworkPacket::operator>>(std::string& dst)
{
	int len = 0;
	*this >> len;

	checkReadBounds(len);

	readString(dst, &data[offset], len);
	offset += len;

	return *this;
}

NetworkPacket& NetworkPacket::operator<<(const glm::vec3& src)
{
	checkWriteBounds(sizeof(float) * 3);

	writeFloat(&data[offset], src.x);
	offset += sizeof(float);

	writeFloat(&data[offset], src.y);
	offset += sizeof(float);

	writeFloat(&data[offset], src.z);
	offset += sizeof(float);

	return *this;
}

NetworkPacket& NetworkPacket::operator>>(glm::vec3& dst)
{
	checkReadBounds(sizeof(float) * 3);

	readFloat(dst.x, &data[offset]);
	offset += sizeof(float);

	readFloat(dst.y, &data[offset]);
	offset += sizeof(float);

	readFloat(dst.z, &data[offset]);
	offset += sizeof(float);

	return *this;
}

void NetworkPacket::checkReadBounds(int size)
{
	if (offset + size > dataSize)
	{
		throw std::exception();
	}
}

void NetworkPacket::checkWriteBounds(int size)
{
	if (offset + size > dataSize)
	{
		dataSize = offset + size;
		data.resize(dataSize);
	}
}
