#include "Message.h"

#include <vector>

bool ClientConnectionRequestMessage::serialize(void* data)
{
	char dataArray[512];

	int clientNameLength = clientName.size() + 1;
	const char* clientNameC = clientName.c_str();

	memcpy(dataArray, &clientId, sizeof(int));
	memcpy(dataArray + sizeof(int), &clientNameLength, sizeof(int));
	strcpy_s(dataArray + sizeof(int) * 2, clientNameLength, clientNameC);

	memcpy(data, dataArray, 512);

	return true;
}

bool ClientConnectionRequestMessage::deserialzie(const void* data)
{
	char dataArray[512];
	memcpy(dataArray, data, 512);

	memcpy(&clientId, dataArray, sizeof(int));
	
	int stringLen = 0;
	memcpy(&stringLen, dataArray + sizeof(int), sizeof(int));
	char* string = new char[stringLen + 1];
	strcpy_s(string, stringLen, dataArray + sizeof(int) * 2);

	clientName = string;
	delete[] string;

	return true;
}
