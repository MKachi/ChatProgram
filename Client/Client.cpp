#include "Client.h"
#include <iostream>
#include "../Server/Protocol.h"

Client::Client(Service& service)
	: _service(service)
	, _socket(service)
	, _login(false) { }

Client::~Client() { }

void Client::receive()
{
	_mutex.lock();

	_socket.async_read_some(
		boost::asio::buffer(_receiveBuffer),
		boost::bind(&Client::receiveHandle, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred)
	);

	_mutex.unlock();
}

void Client::processPacket(const char* data)
{
	Packet* packet = (Packet*)data;
	switch (packet->type)
	{
	case PacketType::Connect_Success:
	{
		Connect_Success* connect = (Connect_Success*)packet;
		_login = connect->isSuccess;
		std::cout << "로그인 성공 반환 값 : " << connect->isSuccess << std::endl;
	}
		break;
	case PacketType::Message:
	{
		Message* message = (Message*)packet;
		if (message->notice)
		{
			std::cout << "[공지] ";
		}
		std::cout << message->name << " : " << message->message << std::endl;
	}
	default:
		break;
	}
}

void Client::connectHandle(const ErrorCode& error)
{
	if (error)
	{
		std::cout << "[Error] " << error.message() << "(code : " << error.value() << ")" << std::endl;
		return;
	}

	std::cout << "서버 접속 완료" << std::endl;
	receive();
}

void Client::writeHandle(const ErrorCode& error, uint bytesTransferrend)
{
	_mutex.lock();
	delete[] _sendDataQueue.front();
	_sendDataQueue.pop();

	char* data = nullptr;

	if (!_sendDataQueue.empty())
	{
		data = _sendDataQueue.front();
	}
	_mutex.unlock();

	if (data != nullptr)
	{
		Packet* packet = (Packet*)data;
		send(packet->size, data, true);
	}
}

void Client::receiveHandle(const ErrorCode& error, uint bytesTransferrend)
{
	if (error)
	{
		if (error == boost::asio::error::eof)
		{
			std::cout << "서버와의 연결이 끊어졌습니다." << std::endl;
		}
		std::cout << "[Error] " << error.message() << "(code : " << error.value() << ")" << std::endl;
		close();
		return;
	}
	memcpy(&_packetBuffer[_packetMark], _receiveBuffer.data(), bytesTransferrend);
	int packetData = _packetMark + bytesTransferrend;
	int readData = 0;

	while (packetData > 0)
	{
		if (packetData <= sizeof(Packet))
		{
			break;
		}

		Packet* packet = (Packet*)&_packetBuffer[readData];
		if (packet->size <= packetData)
		{
			processPacket(&_packetBuffer[readData]);
			packetData -= packet->size;
			readData += packet->size;
		}
		else
		{
			break;
		}
	}

	if (packetData > 0)
	{
		char tempBuffer[RECEIVE_BUFFER_MAX] = { 0, };
		memcpy(&tempBuffer[0], &_packetBuffer[readData], packetData);
		memcpy(&_packetBuffer[0], &tempBuffer[0], packetData);
	}
	_packetMark = packetData;
	receive();
}

void Client::connect(TcpEndPoint endPoint)
{
	_packetMark = 0;
	_socket.async_connect(endPoint,
		boost::bind(&Client::connectHandle, this,
			boost::asio::placeholders::error));
}

void Client::send(const int size, char* data, const bool straight)
{
	char* buffer = nullptr;
	_mutex.lock();
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

	if (straight || !_sendDataQueue.empty())
	{
		boost::asio::async_write(_socket, boost::asio::buffer(buffer, size),
			boost::bind(&Client::writeHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
	_mutex.unlock();
}

void Client::close()
{
	if (_socket.is_open())
	{
		_socket.close();
	}
}