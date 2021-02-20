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

bool EntityInfoMessage::serialize(void* data)
{
	char dataArray[512];

	int nameLen = name.size() + 1;
	memcpy(dataArray, &nameLen, sizeof(int));

	const char* namePtr = name.c_str();
	strcpy_s(dataArray + sizeof(int), nameLen, namePtr);

	int idLen = id.size() + 1;
	memcpy(dataArray + sizeof(int) + nameLen, &idLen, sizeof(int));

	const char* idPtr = id.c_str();
	strcpy_s(dataArray + sizeof(int) * 2 + nameLen, idLen, idPtr);

	memcpy(dataArray + sizeof(int) * 2 + nameLen + idLen, &x, sizeof(float));
	memcpy(dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float), &y, sizeof(float));
	memcpy(dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 2, &z, sizeof(float));
	memcpy(dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 3, &pitch, sizeof(float));
	memcpy(dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 4, &yaw, sizeof(float));
	memcpy(dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 5, &roll, sizeof(float));

	memcpy(data, dataArray, 512);

	return true;
}

bool EntityInfoMessage::deserialzie(const void* data)
{
	char dataArray[512];
	memcpy(dataArray, data, 512);

	int nameLen = 0;
	memcpy(&nameLen, dataArray, sizeof(int));

	char* nameStr = new char[nameLen + 1];
	strcpy_s(nameStr, nameLen, dataArray + sizeof(int));
	name = nameStr;

	int idLen = 0;
	memcpy(&idLen, dataArray + sizeof(int) + nameLen, sizeof(int));

	char* idStr = new char[idLen + 1];
	strcpy_s(idStr, idLen, dataArray + sizeof(int) * 2 + nameLen);
	id = idStr;
	
	memcpy(&x, dataArray + sizeof(int) * 2 + nameLen + idLen, sizeof(float));
	memcpy(&y, dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float), sizeof(float));
	memcpy(&z, dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 2, sizeof(float));
	memcpy(&pitch, dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 3, sizeof(float));
	memcpy(&yaw, dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 4, sizeof(float));
	memcpy(&roll, dataArray + sizeof(int) * 2 + nameLen + idLen + sizeof(float) * 5, sizeof(float));

	return true;
}
