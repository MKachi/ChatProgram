#pragma once

#include "Type.h"
#include <array>
#include <queue>

enum { RECEIVE_BUFFER_MAX = 512 };

class Server;
class Client
{
private:
	uint		_id;
	TcpSocket	_socket;
	Server*		_server;
	std::array<char, RECEIVE_BUFFER_MAX>		_receiveBuffer;
	std::array<char, RECEIVE_BUFFER_MAX * 2>	_packetBuffer;

	int _packetMark;

	std::queue<char*> _sendDataQueue;

	void receiveHandle(const ErrorCode& error, uint bytesTransferred);
	void writeHandle(const ErrorCode& error, uint bytesTransferred);

public:
	Client(int id, Service& service, Server* server);
	~Client();

	void receive();
	void send(const int size, char* data, bool straight = false);

	uint getId() const { return _id; }
	TcpSocket& getSocket() { return _socket; }

};