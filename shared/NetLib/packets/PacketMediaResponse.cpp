#include "PacketMediaResponse.h"


PacketMediaResponse::PacketMediaResponse(PacketType packetType) : Packet(packetType)
{
	if (packetType != PacketType::Desktop && packetType != PacketType::Camera)
	{
		throw "PacketType Must be either Desktop or Camera";
	}
	imageBytes = nullptr;
	imageSize = 0;
}

PacketMediaResponse::~PacketMediaResponse()
{
	delete[] imageBytes;
}

void PacketMediaResponse::calculatePayloadLength()
{
	if (imageSize > 0 && imageBytes != nullptr)
	{
		payloadLength += sizeof int64_t;
		payloadLength += imageSize * sizeof(char);
	}
}

size_t PacketMediaResponse::getPayloadLength()
{
	if (payloadLength == 0)
		calculatePayloadLength();
	return payloadLength;
}

void PacketMediaResponse::serializePayload()
{
	if (imageBytes != nullptr)
	{
		buffer.writeInt64(imageSize);
		buffer.writeBytes(imageBytes, imageSize);
	}
}

PacketMediaResponse* PacketMediaResponse::deserialize(PacketType packetType, char* payloadBuffer, size_t bufferlen)
{
	PacketMediaResponse* packet = nullptr;

	if (bufferlen > 0)
	{
		CursorBuffer cursorBuffer(payloadBuffer, bufferlen, false);
		packet = new PacketMediaResponse(packetType);
		packet->imageSize = cursorBuffer.readInt64();
		packet->imageBytes = new char[packet->imageSize];
		cursorBuffer.readBytes(packet->imageBytes, packet->imageSize);
	}

	return packet;
}
