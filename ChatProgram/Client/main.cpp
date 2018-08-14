#include <thread>
#include "Client.h"
#include <iostream>
#include "Protocol.h"

enum
{
	PORT = 3000
};

int main()
{
	Service service;
	TcpEndPoint endPoint(boost::asio::ip::address::from_string("127.0.0.1"), PORT);

	Client client(service);
	client.connect(endPoint);

	std::thread thread([&]() { service.run(); });

	char inputMessage[200 * 2] = { 0, };
	while (std::cin.getline(inputMessage, 200))
	{
		if (strnlen_s(inputMessage, 200) == 0)
		{
			break;
		}

		if (client.isLogin() == false)
		{
			Connect sendPacket;
			sendPacket.init();

			client.send(sendPacket.size, (char*)&sendPacket);
		}
		else
		{
			Message sendPacket;
			sendPacket.init();

			strncpy_s(sendPacket.message, 200, inputMessage, 200 - 1);
			client.send(sendPacket.size, (char*)&sendPacket);
		}
	}

	service.stop();
	client.close();
	thread.join();

	std::cout << "클라이언트 종료..." << std::endl;
	getchar();

	return 0;
}