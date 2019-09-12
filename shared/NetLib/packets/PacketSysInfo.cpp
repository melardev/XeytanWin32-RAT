#include "PacketSysInfo.h"


PacketSysInfo::PacketSysInfo(char* pcName, char* userName)
	: pcName(pcName), userName(userName)
{
	payloadLength =
		sizeof int32_t + strlen(pcName) +
		sizeof int32_t + strlen(userName);
}

PacketSysInfo::~PacketSysInfo()
{
	delete[] pcName;
	delete[] userName;
}

void PacketSysInfo::serializePayload()
{
	const int32_t pcNameLen = strlen(pcName);
	buffer.writeInt32(pcNameLen);
	buffer.writeBytes(pcName, pcNameLen);

	const int32_t userNameLen = strlen(userName);
	buffer.writeInt32(userNameLen);
	buffer.writeBytes(userName, userNameLen);
}

PacketSysInfo* PacketSysInfo::deserialize(char* payloadData, size_t payloadLength)
{
	CursorBuffer cursorBuffer = CursorBuffer(payloadData, payloadLength, false);

	int32_t propertySize = cursorBuffer.readInt32();
	char* pcName = new char[propertySize + 1];
	cursorBuffer.readBytes(pcName, propertySize);
	pcName[propertySize] = 0x00;

	propertySize = cursorBuffer.readInt32();
	char* userName = new char[propertySize + 1];
	cursorBuffer.readBytes(userName, propertySize);
	userName[propertySize] = 0x00;

	return new PacketSysInfo(pcName, userName);
}
