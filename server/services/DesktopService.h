#pragma once
#include <Windows.h>
#include <utility>

class DesktopService
{
public:
	DesktopService();
	~DesktopService();
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	static void shutdownGdiPlus();
	static bool initGdiPlusIfNeeded();
	static void saveBmpFromByteArray(void* buffer, ULONGLONG bufferLen, const wchar_t* dstPath);
	static std::pair<ULONGLONG, char*> getScreenshotAsByteArray();

private:
};
