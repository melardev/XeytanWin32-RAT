#include <string>
#include "StringService.h"


StringService::StringService()
{
}

StringService::~StringService()
{
}

const wchar_t* StringService::chartoWchar(char* str)
{
	const size_t strSize = strlen(str) + 1;
	size_t convertedChars = 0;
	wchar_t* result = new wchar_t[strSize];
	mbstowcs_s(&convertedChars, result, strSize, str, _TRUNCATE);

	// const size_t size = strlen(str) + 1;	
	// mbstowcs(result, str, size);
	return result;
}

std::wstring StringService::chartoWString(char* str)
{
	const wchar_t* wcharstr = chartoWchar(str);
	std::wstring result(wcharstr);
	delete[] wcharstr;
	return result;
}

std::wstring StringService::stringToWString(const std::string& input)
{
	const int inputLength = (int)input.length() + 1;
	const int len = MultiByteToWideChar(CP_ACP, 0, input.c_str(), inputLength, 0, 0);
	std::wstring output(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, input.c_str(), inputLength, &output[0], len);
	return output;
}

std::string StringService::wstringToString(const std::wstring& input)
{
	const int inputLength = (int)input.length() + 1;
	const int len = WideCharToMultiByte(CP_ACP, 0, input.c_str(), inputLength, 0, 0, 0, 0);
	std::string output(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, input.c_str(), inputLength, &output[0], len, 0, 0);
	return output;
}

std::string& StringService::BstrToStdString(const BSTR bstr, std::string& dst, int cp)
{
	if (!bstr)
	{
		// define NULL functionality. I just clear the target.
		dst.clear();
		return dst;
	}

	// request content length in single-chars through a terminating
	//  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
	//  so this will only convert through the first nullchar.
	int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
	if (res > 0)
	{
		dst.resize(res);
		WideCharToMultiByte(cp, 0, bstr, -1, &dst[0], res, NULL, NULL);
	}
	else
	{
		// no content. clear target
		dst.clear();
	}
	return dst;
}

std::string StringService::BstrToStdString(BSTR bstr, int cp)
{
	std::string str;
	BstrToStdString(bstr, str, cp);
	return str;
}

BSTR StringService::wcharToBSTR(const wchar_t* name)
{
	return SysAllocString(name);
}

std::string StringService::getLastErrorAsString()
{
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	DWORD size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

bool StringService::endsWith(std::string const& checkedStr, std::string const& suffix)
{
	if (checkedStr.length() >= suffix.length())
		return checkedStr.compare(checkedStr.length() - suffix.length(), // offset to begin the comparison
		                          suffix.length() // number of chars to compare
		                          , suffix // compare against suffix
		) == 0;
	else
		return false;
}

bool StringService::endsWith(const char* checkedStr, const char* suffix)
{
	const size_t srcLen = strlen(checkedStr);
	const size_t suffixLen = strlen(suffix);
	return srcLen >= suffixLen && strcmp(checkedStr + srcLen - suffixLen, suffix) == 0;
}


bool StringService::startsWith(const char* checkedStr, const char* prefix)
{
	const size_t srcLen = strlen(checkedStr);
	const size_t prefixLen = strlen(prefix);

	return srcLen >= prefixLen && (strncmp(prefix, checkedStr, prefixLen) == 0);
}

void StringService::trim(std::wstring& str, wchar_t delimiter)
{
	std::string::size_type pos = str.find_last_not_of(delimiter);

	if (pos != std::string::npos)
	{
		// If found, delete next character, with that we have our null byte string terminator
		str.erase(pos + 1);
		pos = str.find_first_not_of(delimiter);
		if (pos != std::string::npos)
			str.erase(0, pos); // delete from beginning up to the point where found
	}
	else
	{
		str.erase(str.begin(), str.end());
	}
}

void StringService::trim(std::wstring& str)
{
	trim(str, ' ');
	trim(str, '\n');
	trim(str, '\t');
	trim(str, '\r');
}

bool StringService::equalInsensitive(const char* a, const char* b)
{
	int la = strlen(a);
	int lb = strlen(b);
	if (la != lb) return false;
	for (lb = 0; lb < la; lb++)
	{
		char char1 = toupper(a[lb]);
		char char2 = toupper(b[lb]);
		if (char1 != char2)
			return false;
	}
	return true;
}
