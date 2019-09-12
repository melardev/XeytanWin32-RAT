#include "DesktopService.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

static ULONG_PTR gdiPlusToken;
static Gdiplus::Status success;

DesktopService::DesktopService()
{
	success = Gdiplus::GenericError;
	gdiPlusToken = NULL;
}

DesktopService::~DesktopService()
{
}

int DesktopService::GetEncoderCLSID(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT num = 0;
	UINT size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT i = 0; i < num; ++i)
	{
		if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[i].Clsid;
			free(pImageCodecInfo);
			return i;
		}
	}
	free(pImageCodecInfo);
	return 0;
}

void DesktopService::shutdownGdiPlus()
{
	Gdiplus::GdiplusShutdown(gdiPlusToken);
	gdiPlusToken = NULL;
}

bool DesktopService::initGdiPlusIfNeeded()
{
	// If already initialized then return true
	if (gdiPlusToken != NULL)
		return true;

	static Gdiplus::GdiplusStartupInput gdiPlusStartupInput;


	return (success = GdiplusStartup(&gdiPlusToken, &gdiPlusStartupInput, NULL)) == Gdiplus::Status::Ok;
}

void DesktopService::saveBmpFromByteArray(void* buffer, ULONGLONG bufferLen, const wchar_t* dstPath)
{
	IStream* iStream;
	// Init GdiPlus
	initGdiPlusIfNeeded();

	HRESULT res = CreateStreamOnHGlobal(NULL, true, &iStream);
	ULONG bytesWritten = 0;
	iStream->Write(buffer, bufferLen, &bytesWritten);

	// Seek the cursor to the beginning
	ULARGE_INTEGER pos{0, 0};
	const LARGE_INTEGER pos2{0, 0};
	iStream->Seek(pos2, STREAM_SEEK_SET, &pos);

	// We can convert IStream containing Bitmap to Bitmap object
	CLSID clsid;
	GetEncoderCLSID(L"image/jpeg", &clsid);
	Gdiplus::Bitmap bmp(iStream, NULL);
	bmp.Save(dstPath, &clsid, NULL);

	// Close the stream
	iStream->Release();
}

std::pair<ULONGLONG, char*> DesktopService::getScreenShotAsByteArray()
{
	if (!DesktopService::initGdiPlusIfNeeded())
		return {};

	IStream* iStream;
	HRESULT res = CreateStreamOnHGlobal(NULL, true, &iStream);

	const HDC srcDC = ::GetDC(NULL);
	const int Height = GetSystemMetrics(SM_CYSCREEN);
	const int Width = GetSystemMetrics(SM_CXSCREEN);
	const HDC memDC = CreateCompatibleDC(srcDC);
	const HBITMAP membit = CreateCompatibleBitmap(srcDC, Width, Height);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, membit);
	BitBlt(memDC, 0, 0, Width, Height, srcDC, 0, 0, SRCCOPY);

	Gdiplus::Bitmap bitmap(membit, NULL);
	CLSID clsid;
	GetEncoderCLSID(L"image/jpeg", &clsid);

	// We can save bitmap into IStream
	bitmap.Save(iStream, &clsid, NULL);

	ULARGE_INTEGER pos{0, 0};
	const LARGE_INTEGER pos2{0, 0};
	iStream->Seek(pos2, STREAM_SEEK_SET, &pos);

	ULONG bytesWritten = 0;
	STATSTG statstg;
	iStream->Stat(&statstg, STATFLAG_DEFAULT);
	const ULONGLONG bufferLen = statstg.cbSize.QuadPart;
	char* imageBuffer = new char[bufferLen];
	iStream->Read(imageBuffer, bufferLen, &bytesWritten);
	iStream->Release();

	DeleteObject(memDC);
	DeleteObject(membit);
	::ReleaseDC(NULL, srcDC);

	std::pair<ULONGLONG, char*> result(bufferLen, imageBuffer);
	return result;
}
