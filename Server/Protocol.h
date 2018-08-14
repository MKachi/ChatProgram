#pragma once

#include "Type.h"

enum class PacketType
{
	Connect,
	Connect_Success,
	Message
};

struct Packet
{
	PacketType	type;
	ushort		size;
	virtual void init() = 0;
};

struct Connect
	: public Packet
{
	void init() override
	{
		type = PacketType::Connect;
		size = sizeof(Connect);
	}
};

struct Connect_Success
	: public Packet
{
	bool isSuccess;

	void init() override
	{
		type = PacketType::Connect_Success;
		size = sizeof(Connect_Success);
		isSuccess = false;
	}
};

struct Message
	: public Packet
{
	char name[200];
	char message[200];
	bool notice;

	void init() override
	{
		type = PacketType::Message;
		size = sizeof(Message);
		memset(name, 0, 200);
		memset(message, 0, 200);
		notice = false;
	}
};