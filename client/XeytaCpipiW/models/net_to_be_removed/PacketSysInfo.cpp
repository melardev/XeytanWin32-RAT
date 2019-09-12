#include "PacketSysInfo.h"


PacketSysInfo::PacketSysInfo(SystemInfoStruct* sysInfoStruct) : Packet(PacketType::SysInfo, (void*)sysInfoStruct,
	strlen(sysInfoStruct->computerName) + 1
	+ strlen(sysInfoStruct->userName) + 1
)
{
}

PacketSysInfo::PacketSysInfo(const unsigned packetPayloadLen, void* data)
	: Packet(PacketType::SysInfo, data)
{
}


PacketSysInfo::~PacketSysInfo()
{
}

void PacketSysInfo::serialize(char* buffer)
{
	// all the + 1 below are because of the null terminator we have to account for it.
	Packet::serialize(buffer);

	SystemInfoStruct* d = (SystemInfoStruct*)data;
	// int pcLen = strlen(d->computerName);
	int pcLen = strlen(d->computerName);
	strcpy_s(serializingCursor, pcLen + 1, d->computerName);
	// serializingCursor += strlen(d->computerName);
	serializingCursor += strlen(d->computerName) + 1;

	strcpy_s(serializingCursor, strlen(d->userName) + 1, d->userName);
	serializingCursor += strlen(d->userName) + 1;
}
