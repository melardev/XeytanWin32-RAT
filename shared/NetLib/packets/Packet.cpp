#include "Packet.h"
#include <sstream>

// #include "../net_utils.h"


Packet::Packet(): buffer(DEFAULT_PACKET_LEN_ALLOC)
{
	payloadLength = 0;
}

Packet::Packet(PacketType packetType) : buffer(DEFAULT_PACKET_LEN_ALLOC), packetType(packetType)
{
	payloadLength = 0;
}

Packet::~Packet()
{
}

PacketType Packet::getType() const
{
	return packetType;
}

Buffer Packet::getBuffer()
{
	return buffer;
}


char* Packet::getHeaderBuffer()
{
	return getBufferData();
}

char* Packet::getPayloadBuffer()
{
	return buffer.getData() + PACKET_HEADER_LENGTH;
}

std::size_t Packet::getHeaderLength()
{
	return
		PACKET_TYPE_LENGTH // Packet Type, example 00 00 00 02
		+
		PACKET_SIZE_LENGTH; // Packet Length, example 00 00 00 10
}

bool Packet::hasValidHeader()
{
	int32_t packetPayloadSize = 0;
	int16_t inputPacketType = -1;

	char* headerBuffer = getHeaderBuffer();

	buffer.setReadingCursorPosition(0);
	packetPayloadSize = buffer.readInt32();
	inputPacketType = buffer.readInt16();

	if (inputPacketType >= (int16_t)PacketType::MinPacketTypeValue
		&& inputPacketType <= (int16_t)PacketType::MaxPacketTypeValue
		&& packetPayloadSize >= 0) // Sometimes we may have a 0 length payload
	{
		if (
			( // Check the packet size, for media packets, allow a greater packet size
				(inputPacketType == (int16_t)PacketType::Camera
					|| inputPacketType == static_cast<int16_t>(PacketType::Desktop))
				&& packetPayloadSize < MAX_MEDIA_PACKET_LENGTH
			)
			||
			(
				(inputPacketType != (int16_t)PacketType::Camera
					&& inputPacketType != static_cast<int16_t>(PacketType::Desktop))
				&& packetPayloadSize < MAX_PACKET_LENGTH
			)
		)
		{
			packetType = static_cast<PacketType>(inputPacketType);
			payloadLength = packetPayloadSize;
			buffer.resize(PACKET_HEADER_LENGTH + payloadLength);
			return true;
		}
	}


	payloadLength = 0;
	packetType = (PacketType)0;
	buffer.invalidate();
	return false;
}

size_t Packet::getPayloadLength()
{
	// return buffer.getDataLength() - PACKET_HEADER_LENGTH;
	return payloadLength;
}


size_t Packet::getTotalPacketSize()
{
	return PACKET_HEADER_LENGTH + getPayloadLength();
}

char* Packet::getPacketBuffer()
{
	return buffer.getData();
}

char* Packet::getSerialized()
{
	// TODO: send uint64_t for headerLen and not int32_t
	const int32_t payloadLen = getPayloadLength();
	const int32_t bufferLen = getTotalPacketSize();
	buffer.resize(bufferLen);

	// Write payload length as int32_t
	buffer.writeInt32(payloadLen);

	// Write packet type as int32_t
	buffer.writeInt16((int16_t)packetType);

	// delegate the payload serialization to each packet concrete class
	serializePayload();
	return getBufferData();
}

std::string Packet::toString(bool onlyTypeAndSize)
{
	std::stringstream ss;

	ss << "Type " << (int16_t)packetType << "\n";
	ss << "Size " << payloadLength << "\n";
	if (!onlyTypeAndSize)
		ss << "Payload " << std::string(getPayloadBuffer(), payloadLength) << "\n";

	return ss.str();
}

void Packet::invalidate()
{
	buffer.invalidate();
}

void Packet::resizeBuffer()
{
	int32_t packetSize = *(int32_t*)getHeaderBuffer();
	buffer.resize(packetSize + PACKET_HEADER_LENGTH);
}

unsigned long long Packet::getDestination() const
{
	return clientId;
}

void Packet::setDestination(unsigned long long clientId)
{
	this->clientId = clientId;
}

char* Packet::getBufferData()
{
	return buffer.getData();
}
