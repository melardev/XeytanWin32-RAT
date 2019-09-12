#pragma once
#include "Packet.h"
#include <vector>


class PacketFileSystem : public Packet
{
public:
	PacketFileSystem();
	~PacketFileSystem();

	size_t getPayloadLength() override;
	void setPayloadLength(int payloadLength);
	void calculatePayloadLength();
	static PacketFileSystem* deserialize(char* buffer, size_t payloadLen);
	std::wstring path;

	std::vector<FileInfo> files;

protected:
	virtual void serializePayload() override;
private:
};
