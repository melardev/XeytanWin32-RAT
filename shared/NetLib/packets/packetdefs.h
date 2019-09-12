#pragma once
#ifndef PACKETTYPE_H
#define PACKETTYPE_H
#include <string>

#define PACKET_SIZE_LENGTH sizeof int32_t
#define PACKET_TYPE_LENGTH sizeof int16_t
#define PACKET_HEADER_LENGTH (PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH)

#define DEFAULT_PACKET_LEN_ALLOC 1024

#define MEGABYTE (1 * 1048576)
#define MAX_PACKET_LENGTH (MEGABYTE * 2)

enum class PacketType
{
	Presentation,
	ClientInformation,
	ProcessList,
	ReverseShell,
	Chat,
	KeyLogger,
	FileSystem,
	Download,
	DesktopConfig,
	Desktop,
	CameraConfig,
	Camera,
	Close,
	Uninstall,
	MinPacketTypeValue = Presentation,
	MaxPacketTypeValue = Uninstall,
};

enum class PacketShellAction : int8_t
{
	Invalid = -1,
	Start,
	PushCommand,
	MoreData = PushCommand,
	Stop
};

#define MAX_MEDIA_PACKET_LENGTH (MEGABYTE * 10) // 10 MB is the max packet size

struct PacketStruct
{
	PacketType packetType;
	unsigned int packetSize;
	//char* data;
	std::string data;
};


// typedef unsigned long DWORD;

struct FileInfo
{
	std::wstring path;
	unsigned long dwFileAttributes;
	unsigned long long fileSize;
};

enum class FileSystemContentType
{
	Roots,
	Files
};

class Process
{
public:
	Process() = default;
	~Process() = default;

	std::string path;
	int32_t pid = 0;
	bool hasUi = false;
	std::string title;
};

#endif
