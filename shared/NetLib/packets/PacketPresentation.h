#pragma once
#include "Packet.h"

class PacketPresentation : public Packet
{
public:
	PacketPresentation();
	PacketPresentation(char* pcName, char* username);
	~PacketPresentation();
	void serializePayload() override;
	
	size_t getPayloadLength() override;
	
	void setUsername(char* username);
	std::string toString(bool onlyTypeAndSize) override;

	char* getUsername();
	char* getPcname();
	static PacketPresentation* deserialize(char* payloadBuffer, size_t payloadLen);

	char* username;
	char* pcName;
};

