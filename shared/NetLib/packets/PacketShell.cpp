#include "PacketShell.h"


PacketShell::PacketShell(): Packet(PacketType::ReverseShell)
{
}

PacketShell::~PacketShell()
{
}

PacketShell::PacketShell(PacketShellAction action, std::string data): Packet(PacketType::ReverseShell),
                                                                      data(std::move(data)),
                                                                      action(action)
{
	payloadLength =
		sizeof PacketShellAction +
		sizeof int32_t + data.size();
}

void PacketShell::serializePayload()
{
	buffer.writeInt8((int8_t)action);
	const int32_t dataLen = data.size();
	buffer.writeInt32(dataLen);
	if (dataLen > 0)
		buffer.writeBytes((void*)data.data(), dataLen);
}

/*static*/
PacketShell* PacketShell::deserialize(char* payloadData, size_t payloadLength)
{
	CursorBuffer cursorBuffer = CursorBuffer(payloadData, payloadLength, false);
	const PacketShellAction action = (PacketShellAction)cursorBuffer.readInt8();
	const int32_t propertySize = cursorBuffer.readInt32();
	std::string data;
	if (propertySize > 0)
	{
		char* buff = new char[propertySize + 1];
		cursorBuffer.readBytes(buff, propertySize);
		buff[propertySize] = 0;
		data.append(buff);
		delete[] buff;
		//data.append(std::string(payloadData, sizeof Action + sizeof int32_t, propertySize));
	}

	return new PacketShell(action, data);
}

size_t PacketShell::getPayloadLength()
{
	if (payloadLength == 0)
	{
		payloadLength =
			sizeof PacketShellAction +
			sizeof int32_t + data.size();
	}
	return payloadLength;
}
