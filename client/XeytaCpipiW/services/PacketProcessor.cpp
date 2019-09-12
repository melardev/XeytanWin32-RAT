/*
#include "PacketProcessor.h"


PacketProcessor::PacketProcessor()
{
}


PacketProcessor::~PacketProcessor()
{
}

Packet* PacketProcessor::buildChatPacket(const PacketStruct& packet_struct)
{
	return nullptr;
}


Packet* PacketProcessor::buildPacket(const PacketStruct& packetStruct)
{
	switch (packetStruct.packetType)
	{
	case PacketType::Chat:
		return buildChatPacket(packetStruct);
	default:
		return nullptr;
	}
}
*/