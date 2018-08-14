#include "Client.h"
#include "Server.h"
#include <iostream>	
#include "Protocol.h"

Client::Client(int id, Service& service, Server* server)
	: _id(id)
	, _socket(service)
	, _server(server)
	, _packetMark(0) { }

Client::~Client()
{
	while (!_sendDataQueue.empty())
	{
		delete[] _sendDataQueue.front();
		_sendDataQueue.pop();
	}
}

void Client::receive()
{
	_socket.async_read_some(
		boost::asio::buffer(_receiveBuffer),
		std::bind(&Client::receiveHandle, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);
}

void Client::receiveHandle(const ErrorCode& error, uint bytesTransferred)
{
	if (error)
	{
		if (error == boost::asio::error::eof)
		{
			std::cout << "클라이언트 ID : " << _id << " 의 연결이 끊어졌습니다." << std::endl;
		}
		else
		{
			std::cout << "[Error] " << error.message() << "(code : " << error.value() << ")" << std::endl;
		}
		_server->closeClient(_id);
		return;
	}

	memcpy(&_packetBuffer[_packetMark], _receiveBuffer.data(), bytesTransferred);
	int sendPacketSize = _packetMark + bytesTransferred;
	int readData = 0;

	while (sendPacketSize > 0)
	{
		if (sendPacketSize < sizeof(Packet))
		{
			break;
		}

		Packet* packet = (Packet*)&_packetBuffer[readData];
		if (packet->size <= sendPacketSize)
		{
			_server->processPacket(_id, &_packetBuffer[readData]);
			sendPacketSize -= packet->size;
			readData += packet->size;
		}
		else
		{
			break;
		}
	}

	if (sendPacketSize > 0)
	{
		char tempBuffer[RECEIVE_BUFFER_MAX] = { 0, };
		memcpy(&tempBuffer[0], &_packetBuffer[readData], sendPacketSize);
		memcpy(&_packetBuffer[0], &tempBuffer[0], sendPacketSize);
	}

	_packetMark = sendPacketSize;
	receive();
}

void Client::send(const int size, char* data, bool straight)
{
	char* buffer = nullptr;
	if (straight)
	{
		buffer = data;
	}
	else
	{
		buffer = new char[size];
		memcpy_s(buffer, size, data, size);

		_sendDataQueue.push(buffer);
	}

	if (!straight && _sendDataQueue.size() > 1)
	{
		return;
	}

	boost::asio::async_write(_socket, boost::asio::buffer(buffer, size),
		boost::bind(&Client::writeHandle, this, 
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred)
	);
}

void Client::writeHandle(const ErrorCode& error, uint bytesTransferred)
{
	delete[] _sendDataQueue.front();
	_sendDataQueue.pop();

	if (_sendDataQueue.empty() == false)
	{
		char* data = _sendDataQueue.front();

		Packet* packet = (Packet*)data;
		send(packet->size, data, true);
	}
}