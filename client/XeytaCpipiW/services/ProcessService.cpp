#include "ProcessService.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include "../../../shared/NetLib/packets/packetdefs.h"

BOOL CALLBACK onEnumWindowProc(HWND hwnd, LPARAM lp);

ProcessService::ProcessService()
{
}

ProcessService::~ProcessService()
{
}

DWORD ProcessService::FindBrocessByName(const std::wstring& processName)
{
	PROCESSENTRY32W processInfo;
	processInfo.dwSize = sizeof(processInfo);

	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (processesSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	Process32FirstW(processesSnapshot, &processInfo);
	if (!processName.compare(processInfo.szExeFile))
	{
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}

	while (Process32NextW(processesSnapshot, &processInfo))
	{
		if (!processName.compare(processInfo.szExeFile))
		{
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}

	CloseHandle(processesSnapshot);
	return 0;
}

bool ProcessService::killProcessByName(const std::wstring& processName)
{
	DWORD processId = this->FindBrocessByName(processName);
	return this->killProcessById(processId);
}

bool ProcessService::killProcessById(DWORD processId)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId);
	if (hProcess == NULL)
		return false;

	DWORD exitCode = 0;
	GetExitCodeProcess(hProcess, &exitCode);
	bool result = TerminateProcess(hProcess, exitCode);
	CloseHandle(hProcess);
	return result;
}

bool ProcessService::isProcessRunning(unsigned long pid)
{
	DWORD dwDesiredAccess = SYNCHRONIZE;
	BOOL bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, pid);
	if (hProcess == NULL)
		return false;

	DWORD dwMilliseconds = 0;
	DWORD result = WaitForSingleObject(hProcess, dwMilliseconds);
	CloseHandle(hProcess);
	return result == WAIT_TIMEOUT;
}

bool ProcessService::isRunAsAdmin() throw()
{
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = NULL;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
	{
		dwError = GetLastError();
		goto Cleanup;
	}
Cleanup:
	if (pAdministratorsGroup)
	{
		FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = NULL;
	}
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}
	return fIsRunAsAdmin;
}

bool ProcessService::isProcessElevated() throw()
{
	BOOL fIsElevated = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		dwError = GetLastError();
		goto Cleanup;
	}
	TOKEN_ELEVATION elevation;
	DWORD dwSize;
	if (!GetTokenInformation(hToken, TokenElevation, &elevation,
	                         sizeof(elevation), &dwSize))
	{
		dwError = GetLastError();
		goto Cleanup;
	}
	fIsElevated = elevation.TokenIsElevated;
Cleanup:
	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}
	return fIsElevated;
}
/*static*/
void ProcessService::getProcessList(std::vector<Process>& processes)
{
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32); // If not set the call Process32FirstW fails

	if (Process32First(handle, &pe))
	{
		do
		{
			Process process;
			// We cast it because there is no DataStream >> or << operators to serialize unsigned long which is the
			// data type of pe.th32ProcessID, being sizeof unsigned long = 8 bytes, we should be fine with qint64
			process.pid = pe.th32ProcessID;

			// set at least the file name, if we then have permissions to retrieve the filepath then we overwrite it
			// with file path
			process.path = pe.szExeFile;

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);
			if (hProcess != NULL)
			{
				// We have successfully opened the process for query
				char filePath[MAX_PATH];
				if (GetModuleFileNameExA(hProcess, NULL, filePath, MAX_PATH))
				{
					process.path = filePath;
				}
				CloseHandle(hProcess);
			}


			EnumWindows(onEnumWindowProc, (LPARAM)&process);
			processes.push_back(process);
		}
		while (Process32Next(handle, &pe));
	}
}


BOOL CALLBACK onEnumWindowProc(HWND hwnd, LPARAM lp)
{
	Process* process = (Process*)lp;
	process->hasUi = IsWindowVisible(hwnd);

	int bufferLength = GetWindowTextLengthA(hwnd);
	char* buffer = new char[bufferLength + 1];
	GetWindowTextA(hwnd, buffer, bufferLength + 1);
	process->title = buffer;
	delete[] buffer;
	return TRUE;
}
