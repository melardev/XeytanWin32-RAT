#pragma once
#include "Packet.h"

class PacketShell : public Packet
{
public:
	
	PacketShell();
	~PacketShell();
	PacketShell(PacketShellAction action, std::string data);
	void serializePayload() override;
	static PacketShell* deserialize(char* payloadData, size_t payloadLength);
	size_t getPayloadLength() override;

public:
	std::string data;
	PacketShellAction action;
};
