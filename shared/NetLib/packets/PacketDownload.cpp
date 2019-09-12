#include "PacketDownload.h"
#include <iostream>

PacketDownload::PacketDownload() : Packet(PacketType::Download)
{
	fileBytes = nullptr;
}

PacketDownload::~PacketDownload()
{
	std::cout << "Packet Download Destructor called" << std::endl;
	delete fileBytes;
}

void PacketDownload::calculatePayloadLength()
{
	payloadLength = 0;
	payloadLength += sizeof int32_t; // FilePath size
	// FilePath bytes count
	payloadLength += filePath.size() * sizeof(wchar_t);

	if (fileBytes != nullptr)
	{
		payloadLength += sizeof int64_t; // Write file size
		payloadLength += fileSize; // File binary data
	}
}

size_t PacketDownload::getPayloadLength()
{
	if (payloadLength == 0)
		calculatePayloadLength();
	return payloadLength;
}

void PacketDownload::serializePayload()
{
	buffer.writeInt32(filePath.size());
	buffer.writeBytes((void*)filePath.c_str(), filePath.size() * sizeof(wchar_t));

	if (fileBytes != nullptr)
	{
		buffer.writeInt64(fileSize);
		buffer.writeBytes(fileBytes, fileSize);
	}
}

PacketDownload* PacketDownload::deserialize(char* payloadBuffer, size_t payloadLength)
{
	CursorBuffer cursorBuffer(payloadBuffer, payloadLength, false);
	auto packet = new PacketDownload;

	const int32_t filePathSize = cursorBuffer.readInt32();
	wchar_t* filePath = new wchar_t[filePathSize + 1];
	cursorBuffer.readBytes(filePath, filePathSize * sizeof(wchar_t));
	filePath[filePathSize] = 0x00;
	packet->filePath = filePath;
	delete[] filePath;

	if (cursorBuffer.getReadingCursorPosition() < payloadLength)
	{
		packet->fileSize = cursorBuffer.readInt64();
		packet->fileBytes = new char[packet->fileSize];
		cursorBuffer.readBytes(packet->fileBytes, packet->fileSize);
	}
	return packet;
}
