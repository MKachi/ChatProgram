#include "Server.h"

enum
{
	PORT = 3000, 
	MAX_CLIENT = 100
};

int main()
{
	Service		service;

	Server server(service, PORT);
	server.init(MAX_CLIENT);
	server.run();

	service.run();

	getchar();

	return 0;
}