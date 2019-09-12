#pragma once
#include "Packet.h"
#include "../../services/SystemInfoService.h"

class PacketSysInfo : public Packet
{
public:
	PacketSysInfo();
	PacketSysInfo(SystemInfoStruct* sysInfoStruct);
	PacketSysInfo(const unsigned packetPayloadLen, void* const data);
	~PacketSysInfo();


	void serialize(char* buffer) override;
	
};
