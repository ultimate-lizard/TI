#pragma once

#include <string>

const int MSG_CLIENT_CONNECTION_REQUEST = 0;
const int MSG_ENTITY_INFO = 1;

class Buffer
{
public:
	Buffer(int size);
	Buffer(const char* buffer, int size);

	void reset(const char* buffer, int size);

	bool readInt(int& dest);
	bool writeInt(int src);

	bool readFloat(float& dest);
	bool writeFloat(float src);

	bool readString(std::string& dest);
	bool writeString(const std::string& src);

	int getBufferSize() const;

	const char* getBuffer() const;

private:
	char* buffer;
	int maxSize;
	int index;
};
