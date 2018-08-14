#include "Server.h"
#include "Protocol.h"

Server::Server(Service& service, const int port)
	: _acceptor(service, TcpEndPoint(boost::asio::ip::tcp::v4(), port))
	, _serverFull(false) { }

Server::~Server() { }

bool Server::accept()
{
	if (_clientIds.empty())
	{
		_serverFull = true;
		return false;
	}

	_serverFull = true;
	int id = _clientIds.front();
	_clientIds.pop();

	_acceptor.async_accept(_clients[id]->getSocket(),
		boost::bind(&Server::acceptHandle, this,
			_clients[id], boost::asio::placeholders::error));

	return true;
}

void Server::acceptHandle(Client* client, const ErrorCode& error)
{
	if (error)
	{
		std::cout << "[Error] " << error.message() << "(code : " << error.value() << ")" << std::endl;
		return;
	}

	std::cout << "ID : " << client->getId() << " 접속 성공" << std::endl;

	client->receive();
	accept();
}

void Server::init(const int maxClientCount)
{
	for (int i = 0; i < maxClientCount; ++i)
	{
		Client* client = new Client(i, _acceptor.get_io_service(), this);
		_clients.push_back(client);
		_clientIds.push(i);
	}
}

void Server::run()
{
	std::cout << "서버 시작" << std::endl;
	accept();
}

void Server::closeClient(const int id)
{
	std::cout << "ID : " << id << " 접속 종료" << std::endl;
	_clients[id]->getSocket().close();
	_clientIds.push(id);

	if (_serverFull)
	{
		accept();
	}
}

void Server::processPacket(const int id, const char* data)
{
	Packet* packet = (Packet*)data;

	switch (packet->type)
	{
	case PacketType::Connect:
	{
		std::cout << "클라이언트 로그인 ID : " << id << std::endl;

		Connect_Success sendPacket;
		sendPacket.init();
		sendPacket.isSuccess = true;
		_clients[id]->send(sendPacket.size, (char*)&sendPacket);
	}
		break;
	case PacketType::Message:
	{
		Message* message = (Message*)data;

		Message sendPacket;
		sendPacket.init();

		strncpy_s(sendPacket.name, 200, std::to_string(_clients[id]->getId()).c_str(), 200 - 1);
		strncpy_s(sendPacket.message, 200, message->message, 200 - 1);

		for (uint i = 0; i < _clients.size(); ++i)
		{
			if (_clients[i]->getSocket().is_open())
			{
				_clients[i]->send(sendPacket.size, (char*)&sendPacket);
			}
		}
	}
		break;
	default:
		break;
	}
}