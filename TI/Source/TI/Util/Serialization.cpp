#include "Serialization.h"

#include <cstring>

void writeInt(char* dst, int src)
{
	memcpy(dst, &src, sizeof(int));
}

void readInt(int& dst, const char* src)
{
	memcpy(&dst, src, sizeof(int));
}

void writeFloat(char* dst, float src)
{
	memcpy(dst, &src, sizeof(float));
}

void readFloat(float& dst, const char* src)
{
	memcpy(&dst, src, sizeof(float));
}

void writeString(char * dst, const std::string & src)
{
	strcpy_s(dst, src.size() + 1, src.c_str());
}

void readString(std::string & dst, char * src, int size)
{
	char* str = new char[size];
	strcpy_s(str, size, src);

	dst = str;

	delete[] str;
}
