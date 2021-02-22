#include "ByteBuffer.h"

Buffer::Buffer(int size) :
	buffer(nullptr),
	maxSize(size),
	index(0)
{
	buffer = new char[size];
}

Buffer::Buffer(const char* buffer, int size) :
	buffer(nullptr),
	maxSize(size),
	index(0)
{
	this->buffer = new char[size];
	memcpy(this->buffer, buffer, size);
}

void Buffer::reset(const char* buffer, int size)
{
	maxSize = size;
	index = 0;
	if (this->buffer)
	{
		delete[] this->buffer;
	}

	this->buffer = new char[size];

	memcpy(this->buffer, buffer, size);
}

void Buffer::clear()
{
	index = 0;
	for (int i = 0; i < maxSize; ++i)
	{
		buffer[i] = 0;
	}
}

bool Buffer::readInt(int& dest)
{
	if (index + sizeof(int) >= maxSize)
	{
		return false;
	}

	memcpy(&dest, buffer + index, sizeof(int));
	index += sizeof(int);

	return true;
}

bool Buffer::writeInt(int src)
{
	if (index + sizeof(int) >= maxSize)
	{
		return false;
	}

	memcpy(buffer + index, &src, sizeof(int));
	index += sizeof(int);

	return true;
}

bool Buffer::readFloat(float& dest)
{
	if (index + sizeof(float) >= maxSize)
	{
		return false;
	}

	memcpy(&dest, buffer + index, sizeof(float));
	index += sizeof(float);

	return true;
}

bool Buffer::writeFloat(float src)
{
	if (index + sizeof(float) >= maxSize)
	{
		return false;
	}

	memcpy(buffer + index, &src, sizeof(float));
	index += sizeof(float);

	return true;
}

bool Buffer::readString(std::string& dest)
{
	int len = 0;

	if (!readInt(len))
	{
		return false;
	}

	if (!len)
	{
		return false;
	}

	if (index + len >= maxSize)
	{
		return false;
	}

	char* strBuf = new char[len];

	strcpy_s(strBuf, len, buffer + index);

	dest = strBuf;

	index += len;

	return true;
}

bool Buffer::writeString(const std::string& src)
{
	int len = static_cast<int>(src.size()) + 1;
	if (!writeInt(len))
	{
		return false;
	}

	if (index + len >= maxSize)
	{
		return false;
	}

	const char* srcStr = src.c_str();

	strcpy_s(buffer + index, len, srcStr);
	index += len;

	return true;
}

int Buffer::getBufferSize() const
{
	return maxSize;
}

const char* Buffer::getBuffer() const
{
	return buffer;
}
