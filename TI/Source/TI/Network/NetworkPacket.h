#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <TI/Network/Protocol.h>

class NetworkPacket
{
public:
	NetworkPacket();
	NetworkPacket(PacketId packetId);
	NetworkPacket(const char* data, int size, int peerId);

	void reset(const char* data, int size, int peerId);

	void setPacketId(PacketId id);
	PacketId getPacketId() const;
	int getPeerId() const;
	size_t getSize() const;
	const char* getData() const;

	NetworkPacket& operator<<(int src);
	NetworkPacket& operator>>(int& dst);
	NetworkPacket& operator<<(float src);
	NetworkPacket& operator>>(float& dst);
	NetworkPacket& operator<<(const std::string& src);
	NetworkPacket& operator>>(std::string& dst);
	NetworkPacket& operator<<(const glm::vec3& src);
	NetworkPacket& operator>>(glm::vec3& dst);

private:
	void checkReadBounds(int size);
	void checkWriteBounds(int size);

private:
	PacketId id;
	int peerId;
	std::vector<char> data;
	size_t dataSize;
	size_t offset;
};
