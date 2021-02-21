#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <array>
#include <variant>

const int MSG_CLIENT_CONNECTION_REQUEST = 0;
const int MSG_ENTITY_INFO = 1;

class Buffer
{
public:
	Buffer(size_t size) : buffer(nullptr), size(size), writeIndex(0), readIndex(0)
	{
		buffer = new char[size];
	}

	Buffer(const char* buffer, size_t size) : buffer(nullptr), size(size), writeIndex(0), readIndex(0)
	{
		this->buffer = new char[size];
		memcpy(this->buffer, buffer, size);
	}

	bool readInt(int& dest)
	{
		if (readIndex + sizeof(int) >= size)
		{
			return false;
		}

		memcpy(&dest, buffer + readIndex, sizeof(int));
		readIndex += sizeof(int);

		return true;
	}

	bool writeInt(int src)
	{
		if (writeIndex + sizeof(int) >= size)
		{
			return false;
		}

		memcpy(buffer + writeIndex, &src, sizeof(int));
		writeIndex += sizeof(int);

		return true;
	}

	bool readFloat(float& dest)
	{
		if (readIndex + sizeof(float) >= size)
		{
			return false;
		}

		memcpy(&dest, buffer + readIndex, sizeof(float));
		readIndex += sizeof(float);

		return true;
	}

	bool writeFloat(float src)
	{
		if (writeIndex + sizeof(float) >= size)
		{
			return false;
		}

		memcpy(buffer + writeIndex, &src, sizeof(float));
		writeIndex += sizeof(float);

		return true;
	}

	bool readString(std::string& dest)
	{
		int len = 0;
		if (!readInt(len))
		{
			return false;
		}

		if (readIndex + len >= size)
		{
			return false;
		}

		char* strBuf = new char[len];

		strcpy_s(strBuf, len, buffer + readIndex);

		dest = strBuf;

		readIndex += len;

		return true;
	}

	bool writeString(const std::string& src)
	{
		int len = static_cast<int>(src.size()) + 1;
		if (!writeInt(len))
		{
			return false;
		}

		if (writeIndex + len >= size)
		{
			return false;
		}

		const char* srcStr = src.c_str();

		strcpy_s(buffer + writeIndex, len, srcStr);
		writeIndex += len;

		return true;
	}

	size_t getBufferSize() const
	{
		return size;
	}

	const char* getBuffer() const
	{
		return buffer;
	}

private:
	char* buffer;
	size_t size;
	size_t writeIndex;
	size_t readIndex;
};

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

//
//class NetMessage
//{
//public:
//	void addMember(std::variant<int, float, std::string> member);
//
//	void serialize(char* data, int len);
//	void deserialize(const char* data, int len);
//
//private:
//	std::vector<std::variant<int, float, std::string>> members;
//};



std::variant<ClientConnectionRequestMessage, EntityInfoMessage> deserializeNetMsg(const char* src, int size);
void serializeNetMsg(std::variant<ClientConnectionRequestMessage, EntityInfoMessage> msg, char* dest, int size);
