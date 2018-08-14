#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <boost/bind.hpp>
#include "Client.h"

class Server
{
private:
	TcpAcceptor _acceptor;
	std::vector<Client*> _clients;
	std::queue<int> _clientIds;

	bool _serverFull;

	bool accept();
	void acceptHandle(Client* client, const ErrorCode& error);

public:
	Server(Service& service);
	~Server();

	void init(const int maxClientCount);
	void run();

	void closeClient(const int id);
	void processPacket(const int id, const char* data);

};