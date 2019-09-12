#pragma once
#include "Packet.h"

class PacketDownload : public Packet
{
public:
	PacketDownload();
	~PacketDownload();

	static PacketDownload* deserialize(char* payloadBuffer, size_t payloadLength);

	void calculatePayloadLength();
	size_t getPayloadLength() override;

protected:
	void serializePayload() override;

public:
	std::wstring filePath;
	char* fileBytes;
	unsigned long long fileSize;
};
