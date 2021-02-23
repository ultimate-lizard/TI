#pragma once

#include <string>
#include <thread>
#include <map>
#include <functional>
#include <variant>

#include <SDL_net.h>

#include <TI/Network/Protocol.h>
#include <TI/Network/NetworkPacket.h>

class Socket
{
	friend class NetworkHandler;

public:
	Socket() : socket(nullptr) {}

	void send(NetworkPacket& packet);
	void receive(NetworkPacket& packet);

	Socket acceptConnection();

	void close();

	inline bool operator==(const Socket& other) const
	{
		return socket == other.socket;
	}

	inline operator bool()
	{
		return socket != nullptr;
	}

private:
	Socket(TCPsocket socket) : socket(socket) {}

private:
	TCPsocket socket;
};

class NetworkHandler
{
public:
	NetworkHandler();
	~NetworkHandler();

	Socket openConnection(int port);
	void closeConnection(Socket socket);

	Socket connect(const std::string& ip, int port);

	// void send(const void* data, int length);


	// void registerMessageReceiver(std::function<void(const void*, const int&)> function);

private:
	// void waitForMessage(TCPsocket socket);
	//void waitForConnection();

	//void onMessageReceive(void* data, int size);
	//void onConnectionReceive(TCPsocket client);

private:
	// std::map<int, TCPsocket> sockets;
	// std::vector<TCPsocket> clients;

	// bool running;

	//std::thread listenThread;
	//std::thread messageThread;

	// std::vector<std::function<void(const void*, const int&)>> messageReceivers;
};
