#include "PacketPresentation.h"
#include <sstream>


PacketPresentation::PacketPresentation() : Packet(PacketType::Presentation)
{
	username = nullptr;
	pcName = nullptr;

	payloadLength = sizeof(int32_t) * 2;
}

PacketPresentation::PacketPresentation(char* pcName, char* username)
	: Packet(PacketType::Presentation), username(username), pcName(pcName)
{
	payloadLength =
		sizeof int32_t + strlen(pcName) +
		sizeof int32_t + strlen(username);
}

PacketPresentation::~PacketPresentation()
{
	delete[] username;
	delete[] pcName;
}

size_t PacketPresentation::getPayloadLength()
{
	return sizeof int32_t + strlen(pcName) +
		sizeof int32_t + strlen(username);
}


void PacketPresentation::setUsername(char* username)
{
	this->username = username;
	payloadLength = strlen(username);
}


std::string PacketPresentation::toString(bool onlyTypeAndSize)
{
	std::stringstream ss;
	ss << Packet::toString(true) << "\n"
		<< "Username: " << username;
	return ss.str();
}

char* PacketPresentation::getUsername()
{
	return username;
}

char* PacketPresentation::getPcname()
{
	return pcName;
}

void PacketPresentation::serializePayload()
{
	const int32_t pcNameLen = strlen(pcName);
	buffer.writeInt32(pcNameLen);
	buffer.writeBytes(pcName, pcNameLen);

	const int32_t userNameLen = strlen(username);
	buffer.writeInt32(userNameLen);
	buffer.writeBytes(username, userNameLen);
}


PacketPresentation* PacketPresentation::deserialize(char* payloadBuffer, size_t payloadLen)
{
	CursorBuffer cursorBuffer(payloadBuffer, payloadLen, false);

	int32_t propertySize = cursorBuffer.readInt32();
	char* pcName = new char[propertySize + 1];
	cursorBuffer.readBytes(pcName, propertySize);
	pcName[propertySize] = 0x00;

	propertySize = cursorBuffer.readInt32();
	char* userName = new char[propertySize + 1];
	cursorBuffer.readBytes(userName, propertySize);
	userName[propertySize] = 0x00;

	return new PacketPresentation(pcName, userName);
}
