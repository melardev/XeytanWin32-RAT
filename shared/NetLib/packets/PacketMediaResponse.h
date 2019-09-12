#pragma once
#include "Packet.h"

class PacketMediaResponse : public Packet
{
public:
	PacketMediaResponse(PacketType packetType);
	~PacketMediaResponse();

	void calculatePayloadLength();
	size_t getPayloadLength() override;
	static PacketMediaResponse* deserialize(PacketType packetType, char* payloadBuffer, size_t bufferlen);
protected:
	void serializePayload() override;
	
public:
	char* imageBytes;
	unsigned long long imageSize;
};
