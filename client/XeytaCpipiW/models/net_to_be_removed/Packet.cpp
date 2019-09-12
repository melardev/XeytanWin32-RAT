#include "Packet.h"


Packet::Packet()
{
}

Packet::Packet(PacketType packet, void* data, unsigned long long len)
	: packetType(packet), data(data), payloadPacketLen(len)
{
}

Packet::Packet(PacketType packetType, void* data) : packetType(packetType), data(data)
{
}


Packet::~Packet()
{
}

PacketType Packet::getType() const
{
	return packetType;
}

UUID* Packet::getDestination()
{
	return destinationUuid;
}

unsigned long long Packet::getPayloadLength()
{
	return getTotalLength() - (sizeof int32_t * 2);
}

void Packet::serialize(char* buffer)
{
	serializingCursor = buffer;

	*((int32_t*)serializingCursor) = htonl((int32_t)getType());
	serializingCursor += sizeof int32_t;

	*((int32_t*)(serializingCursor)) = htonl((int32_t)getPayloadLength());
	serializingCursor += sizeof int32_t;
}

unsigned Packet::getTotalLength() const
{
	// The packet structure looks like this: 
	// FOUR BYTES stating the packet type
	// FOUR BYTES stating the packet length
	// payloadPacketLen BYTES containing the data
	return sizeof int32_t * 2 + payloadPacketLen;
}
