#include "PacketProcess.h"


PacketProcess::PacketProcess() : Packet(PacketType::ProcessList)
{
}

PacketProcess::~PacketProcess()
{
}


void PacketProcess::serializePayload()
{
	buffer.writeInt32(processes.size());
	for (const Process& process : processes)
	{
		buffer.writeInt32(process.pid);

		buffer.writeInt32(process.path.size());
		buffer.writeBytes((void*)process.path.c_str(), process.path.size());

		buffer.writeBool(process.hasUi);

		if (process.hasUi)
		{
			buffer.writeInt32(process.title.size());
			buffer.writeBytes((void*)process.path.c_str(), process.title.size());
		}
	}
}


PacketProcess* PacketProcess::deserialize(char* payloadBuffer, size_t payloadLength)
{
	CursorBuffer cursorBuffer(payloadBuffer, payloadLength, false);

	int32_t processesCount = cursorBuffer.readInt32();
	std::vector<Process> processes;
	for (int i = 0; i < processesCount; i++)
	{
		Process process;

		process.pid = cursorBuffer.readInt32();

		int32_t pathSize = cursorBuffer.readInt32();
		char* path = new char[pathSize + 1];
		cursorBuffer.readBytes(path, pathSize);
		path[pathSize] = 0x00;
		process.path = path;
		delete[] path;

		process.hasUi = cursorBuffer.readBool();

		if (process.hasUi)
		{
			int32_t titleSize = cursorBuffer.readInt32();
			char* title = new char[titleSize + 1];
			cursorBuffer.readBytes(title, titleSize);
			title[titleSize] = 0x00;
			process.title = title;
			delete[] title;
		}

		processes.push_back(process);
	}

	PacketProcess* packet = new PacketProcess;
	packet->processes = processes;
	return packet;
}

size_t PacketProcess::getPayloadLength()
{
	size_t len = 0;
	len += sizeof int32_t;
	for (const Process& process : processes)
	{
		len += sizeof int32_t; // pid
		len += sizeof int32_t + process.path.size();
		len += 1; // has Ui
		if (process.hasUi)
			len += sizeof int32_t + process.title.size();
	}
	return len;
}
