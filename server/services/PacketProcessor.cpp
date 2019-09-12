
#include "PacketProcessor.h"

/*
PacketProcessor::PacketProcessor()
{
}


PacketProcessor::~PacketProcessor()
{
}

Packet* PacketProcessor::buildPacket(Client& client, Packet& packet)
{

	switch (packet.getType())
	{
	case PacketType::Chat:
	{
		return buildChatPacket(packet);
	}
	case PacketType::ClientInformation:
	{
		int pcNameLen = strlen((char*)packetStruct.data);
		char* pcNameBuff = new char[pcNameLen + 1];
		strcpy_s(pcNameBuff, pcNameLen + 1, (char*)packetStruct.data);

		int usernameLen = strlen((char*)packetStruct.data + pcNameLen + 1);
		char* usernameBuff = new char[usernameLen + 1];
		strcpy_s(usernameBuff, usernameLen + 1, (char*)packetStruct.data + pcNameLen + 1);

		char uuidBuffer[256];
		UuidService::getStringFromUuid(uuid, uuidBuffer);
		Client dto(uuidBuffer, pcNameBuff, usernameBuff);

		return new PacketSysInfo(dto);
		// return new PacketSysInfo(packetStruct.packetSize, packetStruct.data);
	}
	default:
		return nullptr;
	}
}

Packet* PacketProcessor::buildChatPacket(const Packet& packet_struct)
{
	return nullptr;
}


/* TODO: Delete me
Packet* PacketProcessor::buildPacket(UUID* uuid, const PacketStruct& packetStruct)
{
	switch (packetStruct.packetType)
	{
	case PacketType::Chat:
		{
			return buildChatPacket(packetStruct);
		}
	case PacketType::SysInfo:
		{
			int pcNameLen = strlen((char*)packetStruct.data);
			char* pcNameBuff = new char[pcNameLen + 1];
			strcpy_s(pcNameBuff, pcNameLen + 1, (char*)packetStruct.data);

			int usernameLen = strlen((char*)packetStruct.data + pcNameLen + 1);
			char* usernameBuff = new char[usernameLen + 1];
			strcpy_s(usernameBuff, usernameLen + 1, (char*)packetStruct.data + pcNameLen + 1);

			char uuidBuffer[256];
			UuidService::getStringFromUuid(uuid, uuidBuffer);
			Client dto(uuidBuffer, pcNameBuff, usernameBuff);

			return new PacketSysInfo(dto);
			// return new PacketSysInfo(packetStruct.packetSize, packetStruct.data);
		}
	default:
		return nullptr;
	}
}

*/