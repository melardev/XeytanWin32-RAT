#pragma once
#include "Packet.h"
#include <vector>

class PacketProcess : public Packet
{
public:
	PacketProcess();
	~PacketProcess();
	static PacketProcess* deserialize(char* payloadBuffer, size_t payloadLength);
protected:
	virtual void serializePayload() override;
	
	size_t getPayloadLength() override;
public:

	std::vector<Process> processes;
};
