#pragma once

#include <boost/asio.hpp>

typedef wchar_t				wchar;
typedef unsigned char		uchar;
typedef unsigned char		byte;
typedef char				sbyte;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef long long			int64;
typedef unsigned long long	uint64;

typedef boost::asio::io_service				Service;
typedef boost::asio::ip::tcp::socket		TcpSocket;
typedef boost::asio::ip::tcp::endpoint		TcpEndPoint;
typedef boost::asio::ip::tcp::acceptor		TcpAcceptor;
typedef boost::system::error_code			ErrorCode;