#pragma once
#include <Windows.h>
#include <utility>


class DesktopService
{
public:
	DesktopService();
	~DesktopService();
	static int GetEncoderCLSID(const wchar_t* format, CLSID* pClsid);
	static void shutdownGdiPlus();
	static bool initGdiPlusIfNeeded();
	static void saveBmpFromByteArray(void* buffer, unsigned long long bufferLen, const wchar_t* dstPath);
	static std::pair<unsigned long long, char*> getScreenShotAsByteArray();

private:
};
