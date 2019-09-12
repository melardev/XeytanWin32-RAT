#pragma once
#include "Packet.h"

class PacketSysInfo : public Packet
{
public:

	PacketSysInfo(const unsigned packetPayloadLen, void* const data);
	PacketSysInfo(char* pcName, char* userName);
	~PacketSysInfo();

	void serializePayload() override;
	static PacketSysInfo* deserialize(char* get_payload_buffer, size_t payloadLength);

	char* pcName;
	char* userName;
};
