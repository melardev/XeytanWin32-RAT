#pragma once
#include "../models/sysinfodef.h"

#include <comdef.h>
#include <WinInet.h>
#include <WbemIdl.h>
#include <conio.h>
#include "../../../shared/IOLib/services/StringService.h"


#define  ENV_VAR_STRING_COUNT  (sizeof(envVarStrings)/sizeof(TCHAR*))



struct SystemInfoStruct
{
	char* computerName;
	char* userName;
};

class SystemInfoService
{
public:
	SystemInfoService();
	~SystemInfoService();

	bool getAntivirusName(AntivirusInfo& antivirusInfo)
	{
		CoInitializeEx(0, 0);
		CoInitializeSecurity(0, -1, 0, 0, 0, 3, 0, 0, 0);
		IWbemLocator* locator = 0;
		CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (void **)&locator);
		IWbemServices* services = 0;

		const wchar_t* name = L"root\\SecurityCenter2";

		if (locator->ConnectServer(StringService::wcharToBSTR(name), 0, 0, 0, 0, 0, 0, &services) >= 0)
		{
			printf("Connected!\n");
			//Lets get system information
			CoSetProxyBlanket(services, 10, 0, 0, 3, 3, 0, 0);
			const wchar_t* query = L"Select * From AntiVirusProduct";

			const wchar_t* queryLanguage = L"WQL";

			IEnumWbemClassObject* e = 0;
			if (				SUCCEEDED(services->ExecQuery(StringService::wcharToBSTR(queryLanguage),
				StringService::wcharToBSTR(query),WBEM_FLAG_FORWARD_ONLY, 0, &e)))
			{
				printf("Query executed successfuly!\n");
				IWbemClassObject* object = 0;
				ULONG u = 0;
				//lets enumerate all data from this table

				std::string antiVirus;

				while (e)
				{
					e->Next(WBEM_INFINITE, 1, &object, &u);
					if (!u) break; //no more data,end enumeration
					VARIANT cvtVersion;
					object->Get(L"displayName", 0, &cvtVersion, 0, 0);

					services->Release();
					locator->Release();
					CoUninitialize();

					std::string dest;
					StringService::BstrToStdString(cvtVersion.bstrVal, dest);
					antivirusInfo.name = dest;
				}
			}
			else
				printf("Error executing query!\n");
		}
		else
			printf("Connection error!\n");

		services->Release();
		locator->Release();
		CoUninitialize();


		return false; // No Antivirus found
	}

	std::string getGlobalIp()
	{
		char buffer[4096];
		DWORD read = this->openUrl("https://api.ipify.org/", buffer);

		return std::string(buffer, read);
	}

	DWORD openUrl(const std::string& url, char* buffer)
	{
		return this->openUrl(url.c_str(), buffer);
	}

	// This goes into InternetService
	DWORD openUrl(const char* url, char* buffer)
	{
		HINTERNET net = InternetOpen("--",
		                             INTERNET_OPEN_TYPE_PRECONFIG,
		                             NULL,
		                             NULL,
		                             0);

		HINTERNET conn = InternetOpenUrl(net,
		                                 url,
		                                 NULL,
		                                 0,
		                                 INTERNET_FLAG_RELOAD,
		                                 0);

		DWORD read;

		InternetReadFile(conn, buffer, sizeof(buffer) / sizeof(buffer[0]), &read);
		InternetCloseHandle(net);

		return read;
	}

	std::string getCmdLocation()
	{
		TCHAR sysAbsolutePath[MAX_PATH];
		char cmdAbsolutePath[MAX_PATH + 7];

		GetSystemDirectory(sysAbsolutePath, MAX_PATH);
		sprintf(cmdAbsolutePath, "%s\\cmd.exe", sysAbsolutePath);
		return std::string(cmdAbsolutePath);
	}

	std::string getUserPath()
	{
		/*
		std::string drivePath = getenv("SystemDrive");
		std::string homePath = getenv("HomePath");
		std::string usersPath = drivePath + homePath;
		return usersPath;
		*/
		return "";
	}

	std::string getExePath()
	{
		TCHAR exeName[MAX_PATH];
		GetModuleFileNameA(NULL, exeName, MAX_PATH);
		std::string exe = exeName;
		return exe;
	}

	BOOL isCurrentUserAdmin()
	{
		BOOL isAdmin = FALSE;
		SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
		PSID pAdministratorsGroup = NULL;
		DWORD dwError = ERROR_SUCCESS;
		if (!AllocateAndInitializeSid(&ntAuthority, 2,
		                              SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
		                              0, 0, 0, 0, 0, 0,
		                              &pAdministratorsGroup))
		{
			dwError = GetLastError();
		}
		else if (!CheckTokenMembership(NULL, pAdministratorsGroup, &isAdmin))
		{
			dwError = GetLastError();
		}

		if (pAdministratorsGroup)
		{
			FreeSid(pAdministratorsGroup);
			pAdministratorsGroup = NULL;
		}

		if (ERROR_SUCCESS != dwError)
		{
			throw dwError;
		}

		return isAdmin;
	}

	void promptForAdminPrivileges()
	{
		BOOL bAlreadyRunningAsAdministrator = FALSE;

		try
		{
			bAlreadyRunningAsAdministrator = isCurrentUserAdmin();
		}
		catch (...)
		{
		}

		if (!bAlreadyRunningAsAdministrator)
		{
			char szPath[MAX_PATH];

			if (GetModuleFileName(NULL, szPath, MAX_PATH))
			{
				SHELLEXECUTEINFO sei = {sizeof(sei)};
				sei.lpVerb = "runas";
				sei.lpFile = szPath;
				sei.hwnd = NULL;
				sei.nShow = SW_NORMAL;
			}
		}
	}

	bool isWin8OrLater()
	{
		/*if (m_osVerInfo.dwOSVersionInfoSize == 0)
		{
			return false;
		}
		return ((m_osVerInfo.dwMajorVersion > 6) || ((m_osVerInfo.dwMajorVersion == 6) && (m_osVerInfo.dwMinorVersion >=
			2)));
		*/
	}

	bool isUserInAdminGroup()
	{
		BOOL isInAdminGroup = FALSE;
		HANDLE hToken, hTokenToCheck;
		DWORD dwError;
		DWORD cbSize = 0;
		OSVERSIONINFOA osVersionInfo = {sizeof(osVersionInfo)};
		bool result = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &hToken);
		if (!result)
		{
			dwError = GetLastError();
			goto cleanup;
		}

		/* TODO: repalce by no deprecated
		if (!GetVersionExA(&osVersionInfo))
		{
			dwError = GetLastError();
			goto cleanup;
		}
		*/
		if (osVersionInfo.dwMajorVersion >= 6)
		{
			TOKEN_ELEVATION_TYPE elevType;
			if (!GetTokenInformation(hToken, TokenElevationType,
			                         &elevType, sizeof(elevType), &cbSize))
			{
				dwError = GetLastError();
				goto cleanup;
			}

			if (TokenElevationTypeLimited == elevType)
			{
				if (!GetTokenInformation(hToken, TokenLinkedToken, &hTokenToCheck,
				                         sizeof(hTokenToCheck), &cbSize))
				{
					dwError = GetLastError();
					goto cleanup;
				}
			}
		}

		if (!hTokenToCheck)
		{
			if (!DuplicateToken(hToken, SecurityIdentification, &hTokenToCheck))
			{
				dwError = GetLastError();
				goto cleanup;
			}
		}

		BYTE adminSID[SECURITY_MAX_SID_SIZE];
		cbSize = sizeof(adminSID);
		if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID,
		                        &cbSize))
		{
			dwError = GetLastError();
			goto cleanup;
		}
		if (!CheckTokenMembership(hTokenToCheck, &adminSID, &isInAdminGroup))
		{
			dwError = GetLastError();
			goto cleanup;
		}

	cleanup:
		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
		if (hTokenToCheck)
		{
			CloseHandle(hTokenToCheck);
			hTokenToCheck = NULL;
		}

		if (dwError != ERROR_SUCCESS)
		{
			throw dwError;
		}

		return isInAdminGroup;
	}

	void getScreenSize(int* size)
	{
		size[0] = GetSystemMetrics(SM_CXSCREEN);
		size[1] = GetSystemMetrics(SM_CYSCREEN);
	}

	float getCpuUsage()
	{
		FILETIME sysIdle, sysKernel, sysUser;
		FILETIME procCreation, procExit, procKernel, procUser;

		if (!GetSystemTimes(&sysIdle, &sysKernel, &sysUser) ||
			!GetProcessTimes(GetCurrentProcess(), &procCreation, &procExit, &procKernel, &procUser))
		{
			// can't get time info so return
			return -1;
		}

		// check for first call
		if (firstGetCpu)
		{
			// save time info before return
			prevSysKernel.dwLowDateTime = sysKernel.dwLowDateTime;
			prevSysKernel.dwHighDateTime = sysKernel.dwHighDateTime;

			prevSysUser.dwLowDateTime = sysUser.dwLowDateTime;
			prevSysUser.dwHighDateTime = sysUser.dwHighDateTime;

			prevProcKernel.dwLowDateTime = procKernel.dwLowDateTime;
			prevProcKernel.dwHighDateTime = procKernel.dwHighDateTime;

			prevProcUser.dwLowDateTime = procUser.dwLowDateTime;
			prevProcUser.dwHighDateTime = procUser.dwHighDateTime;
			firstGetCpu = false;
			return 0;
		}

		ULONGLONG sysKernelDiff = subtractTime(sysKernel, prevSysKernel);
		ULONGLONG sysUserDiff = subtractTime(sysUser, prevSysUser);

		ULONGLONG procKernelDiff = subtractTime(procKernel, prevProcKernel);
		ULONGLONG procUserDiff = subtractTime(procUser, prevProcUser);

		ULONGLONG sysTotal = sysKernelDiff + sysUserDiff;
		ULONGLONG procTotal = procKernelDiff + procUserDiff;

		return (float)((100.0 * procTotal) / sysTotal);
	}

	SystemInfoStruct* getBasicSystemInfo();

	ULONGLONG subtractTime(const FILETIME& a, const FILETIME& b)
	{
		LARGE_INTEGER la, lb;
		la.LowPart = a.dwLowDateTime;
		la.HighPart = a.dwHighDateTime;
		lb.LowPart = b.dwLowDateTime;
		lb.HighPart = b.dwHighDateTime;

		return la.QuadPart - lb.QuadPart;
	}

private:
	FILETIME prevSysKernel;
	FILETIME prevSysUser;
	FILETIME prevProcKernel;
	FILETIME prevProcUser;
	bool firstGetCpu;
};
