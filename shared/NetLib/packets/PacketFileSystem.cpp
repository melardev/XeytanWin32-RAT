#include "PacketFileSystem.h"


PacketFileSystem::PacketFileSystem(): Packet(PacketType::FileSystem)
{
}

PacketFileSystem::~PacketFileSystem() = default;

size_t PacketFileSystem::getPayloadLength()
{
	if (payloadLength == 0)
	{
		calculatePayloadLength();
	}

	return payloadLength;
}

void PacketFileSystem::serializePayload()
{
	// Write std::string path
	buffer.writeInt32(path.size());
	buffer.writeBytes((void*)path.c_str(), path.size() * sizeof(wchar_t));

	buffer.writeInt32(files.size());
	for (const FileInfo& file : files)
	{
		// Write attributes
		buffer.writeInt32(file.dwFileAttributes);

		// Write FileInfo.path
		buffer.writeInt32(file.path.size());
		buffer.writeBytes((void*)file.path.c_str(), file.path.size() * sizeof(wchar_t));

		// Write file size
		// TODO: check if dir or driveType if yes, then exclude fileSize
		buffer.writeInt64(file.fileSize);
	}
}

void PacketFileSystem::setPayloadLength(int payloadLength)
{
	this->payloadLength = payloadLength;
}

void PacketFileSystem::calculatePayloadLength()
{
	size_t len = 0;
	// Size of Path serialization
	len += sizeof int32_t + path.size() * sizeof(wchar_t);

	// Size of list of FileInfo structs serialization
	len += sizeof int32_t;
	for (const FileInfo& fileInfo : files)
	{
		len += sizeof int32_t; // fileAttributes
		len += sizeof int32_t + fileInfo.path.size() * sizeof(wchar_t);
		len += sizeof(unsigned long long); // fileSize
	}

	payloadLength = len;
}

PacketFileSystem* PacketFileSystem::deserialize(char* payloadData, size_t payloadLen)
{
	CursorBuffer cursorBuffer(payloadData, payloadLen, false);
	auto packet = new PacketFileSystem;

	// Read std::string path
	int32_t pathSize = cursorBuffer.readInt32();
	wchar_t* path = new wchar_t[pathSize + 1];
	cursorBuffer.readBytes((void*)path, pathSize * sizeof(wchar_t));
	path[pathSize] = 0x00;
	packet->path = path;
	delete[] path;

	const int32_t filesInfoCount = cursorBuffer.readInt32();
	std::vector<FileInfo> fileInfos;
	for (int i = 0; i < filesInfoCount; i++)
	{
		FileInfo fileInfo;

		fileInfo.dwFileAttributes = cursorBuffer.readInt32();

		// Read path
		pathSize = cursorBuffer.readInt32();
		path = new wchar_t[pathSize + 1];
		cursorBuffer.readBytes(path, pathSize * sizeof(wchar_t));
		path[pathSize] = 0x00;
		fileInfo.path = path;
		delete[] path;

		fileInfo.fileSize = cursorBuffer.readInt64();

		fileInfos.push_back(fileInfo);
	}


	// copy fileInfos into member field
	packet->files = fileInfos;
	return packet;
}
