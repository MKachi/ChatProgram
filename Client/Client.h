#pragma once

#include "Type.h"
#include <queue>

enum { RECEIVE_BUFFER_MAX = 512 };
class Client
{
private:
	Service&	_service;
	TcpSocket	_socket;
	std::array<char, RECEIVE_BUFFER_MAX>		_receiveBuffer;
	std::array<char, RECEIVE_BUFFER_MAX * 2>	_packetBuffer;

	int _packetMark;
	bool _login;

	std::queue<char*> _sendDataQueue;

	void receive();
	void processPacket(const char* data);

	void connectHandle(const ErrorCode& error);
	void writeHandle(const ErrorCode& error, uint bytesTransferrend);
	void receiveHandle(const ErrorCode& error, uint bytesTransferrend);

public:
	Client(Service& service);
	~Client();

	bool isConnect() { return _socket.is_open(); }

	void login() { _login = true; }
	bool isLogin() { return _login; }

	void connect(TcpEndPoint endPoint);
	void send(const int size, char* data, const bool straight = false);

	void close();

};