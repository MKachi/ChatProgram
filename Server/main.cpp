#include "Server.h"

enum
{
	PORT = 3000, 
	MAX_CLIENT = 100
};

int main()
{
	Service		service;
	TcpEndPoint endPoint(boost::asio::ip::tcp::v4(), PORT);

	Server server(service);
	server.init(MAX_CLIENT);
	server.run();

	service.run();

	return 0;
}