#pragma once

#include "Type.h"

enum class PacketType
{

};

struct PacketHeader
{
	PacketType	id;
	ushort		size;
	virtual void init() = 0;
};