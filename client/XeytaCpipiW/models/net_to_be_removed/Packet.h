#pragma once
#include "../../models/packetdefs.h"
#include <rpc.h>


class Packet
{
public:
	Packet();
	Packet(PacketType packet, void* data, size_t payloadPacketLength);
	Packet(PacketType packetType, void* data);
	
	virtual ~Packet();
	PacketType getType() const;
	UUID* getDestination();
	unsigned long long getPayloadLength();
	virtual void serialize(char* buffer);
	unsigned getTotalLength() const;
	PacketType packetType;
	UUID* destinationUuid;

protected:
	void* data;
	char* serializingCursor;
private:
	unsigned int payloadPacketLen;
};
