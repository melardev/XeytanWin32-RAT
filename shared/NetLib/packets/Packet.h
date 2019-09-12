#pragma once

#include "packetdefs.h"
#include "../../IOLib/buffers/CursorBuffer.h"

class Packet
{
public:
	Packet();
	Packet(PacketType packetType);
	virtual ~Packet();
	PacketType getType() const;
	Buffer getBuffer();

	bool hasValidHeader();
	std::size_t getHeaderLength();

	char* getHeaderBuffer();
	char* getPayloadBuffer();

	virtual size_t getPayloadLength();
	size_t getTotalPacketSize();
	char* getPacketBuffer();

	char* getSerialized();
	virtual std::string toString(bool onlyTypeAndSize = true);
	void invalidate();
	void resizeBuffer();

	unsigned long long getDestination() const;
	void setDestination(unsigned long long clientId);
protected:
	virtual void serializePayload()
	{
	};
private:
	char* getBufferData();

protected:
	CursorBuffer buffer;
	unsigned payloadLength;
	PacketType packetType;

	unsigned long long clientId;
};
