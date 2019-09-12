#include "SystemInfoService.h"
#include "../appdefs.h"


SystemInfoService::SystemInfoService()
{
}


SystemInfoService::~SystemInfoService()
{
}

void printError(const wchar_t* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
	              FORMAT_MESSAGE_IGNORE_INSERTS,
	              NULL, eNum,
	              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	              sysMsg, 256, NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while ((*p > 31) || (*p == 9))
		++p;
	do { *p-- = 0; }
	while ((p >= sysMsg) &&
		((*p == '.') || (*p < 33)));

	// Display the message
	wprintf(L"\n\t%s failed with error %d (%s)", msg, eNum, sysMsg);
}

SystemInfoStruct* SystemInfoService::getBasicSystemInfo()
{
	// AntivirusInfo av;
	// getAntivirusName(av);

	SystemInfoStruct* sysInfoStruct = new SystemInfoStruct;

	const wchar_t* envVarStrings[] =
	{
		L"OS         = %OS%",
		L"PATH       = %PATH%",
		L"HOMEPATH   = %HOMEPATH%",
		L"TEMP       = %TEMP%"
	};

	char pcName[INFO_BUFFER_SIZE];
	char userName[INFO_BUFFER_SIZE];

	DWORD bufCharCount = INFO_BUFFER_SIZE;

	// Get and display the name of the computer. 
	bufCharCount = INFO_BUFFER_SIZE;
	if (!GetComputerNameA(pcName, &bufCharCount))
		printError(L"GetComputerName");

	sysInfoStruct->computerName = pcName;

	// Get and display the user name. 
	bufCharCount = INFO_BUFFER_SIZE;
	if (!GetUserNameA(userName, &bufCharCount))
		printError(L"GetUserName");

	sysInfoStruct->userName = userName;

	return sysInfoStruct;
	/*
	// Expand and display a few environment variables. 
	wprintf(L"\n\nSmall selection of Environment Variables:");
	for (i = 0; i < ENV_VAR_STRING_COUNT; ++i)
	{
		bufCharCount = ExpandEnvironmentStringsW(envVarStrings[i], infoBuf,
		                                         INFO_BUFFER_SIZE);
		if (bufCharCount > INFO_BUFFER_SIZE)
			wprintf(L"\n\t(Buffer too small to expand: \"%s\")",
			        envVarStrings[i]);
		else if (!bufCharCount)
			printError(L"ExpandEnvironmentStrings");
		else
			wprintf(L"\n   %s", infoBuf);
	}
	*/
}
