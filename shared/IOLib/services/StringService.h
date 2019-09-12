#pragma once

#include <comdef.h>
#include <string>

class StringService
{
public:
	StringService();
	~StringService();

	static const wchar_t* chartoWchar(char* str);
	static std::wstring chartoWString(char* str);
	static std::wstring stringToWString(const std::string& input);

	static std::string wstringToString(const std::wstring& input);
	static std::string& BstrToStdString(const BSTR bstr, std::string& dst, int cp = CP_UTF8);
	static std::string BstrToStdString(BSTR bstr, int cp);

	static BSTR wcharToBSTR(const wchar_t* name);

	std::string getLastErrorAsString();

	static bool endsWith(std::string const& checkedStr, std::string const& suffix);
	static bool endsWith(const char* checkedStr, const char* suffix);
	static bool startsWith(const char* checkedStr, const char* prefix);

	static void trim(std::wstring& str, wchar_t delimiter);
	static void trim(std::wstring& str);

	static bool equalInsensitive(const char* a, const char* b);

private:
	// MemoryManagementService* memoryService;
};
