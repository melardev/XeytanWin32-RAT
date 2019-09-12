#pragma once
#include "../../../shared/NetLib/packets/packetdefs.h"
#include <fstream>
#include <vector>

class FileSystemService
{
public:
	FileSystemService();
	~FileSystemService();
	static std::vector<std::wstring> GetLocicalDrivesW();

	void hideFile(char* fileAbsolutePath);

	// This should go into MeltService
	void hideSelf();

	// Taken from Ransomware-1
	bool hideSelf2();

	static bool fileExists(const char* fileAbsolutePath);

	static std::ifstream::pos_type getFileSize(const char* filename);
	static std::ifstream::pos_type getFileSize(const std::wstring& filename);

	static void readBinaryFile(const wchar_t* fileName, void* buffer, unsigned long bufferLen);
	static bool writeBinaryFile(const wchar_t* fileName, void* binaryData, unsigned long long binaryDataLen);

	static char* getDesktopPath();

	static wchar_t* getWindirNot();

	bool isDirectoryExisting(const std::wstring& file);

	static bool fileExists(const std::wstring& file);

	bool deleteDir(const wchar_t* path);

	static void listRoots(std::vector<FileInfo>& roots);
	static void listFilesW(const std::wstring& path, std::vector<FileInfo>& files);
};
