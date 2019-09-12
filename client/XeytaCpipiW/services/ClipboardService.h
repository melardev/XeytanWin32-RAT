#pragma once
#include <Windows.h>

class ClipboardService
{
public:
	ClipboardService();
	~ClipboardService();

	const wchar_t* getClipboardText()
	{
		if (OpenClipboard(NULL))
		{
			HANDLE clip = GetClipboardData(CF_UNICODETEXT);
			wchar_t* c;
			c = (wchar_t*)clip;
			CloseClipboard();
			return (wchar_t*)c;
		}
		return L"";
	}

	wchar_t* getClipboardText2()
	{
		wchar_t* wText = NULL;
		if (OpenClipboard(NULL))
		{
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);
			if (hData != NULL)
			{
				size_t sz = GlobalSize(hData);
				if (sz > 0)
				{
					wchar_t* wGlobal = (wchar_t*)GlobalLock(hData);
					wText = new wchar_t[sz];
					wcscpy(wText, wGlobal);
				}
				GlobalUnlock(hData);
			}
			CloseClipboard();
		}
		return wText;
	}

	/*/*__declspec(dllexport)*/
	void setClipboardText(wchar_t* str)
	{
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			HGLOBAL hClipboardData;
			size_t size = (wcslen(str) + 1) * sizeof(wchar_t);
			hClipboardData = GlobalAlloc(NULL, size);
			wchar_t* pchData = (wchar_t*)GlobalLock(hClipboardData);
			memcpy(pchData, str, size);
			SetClipboardData(CF_UNICODETEXT, hClipboardData);
			GlobalUnlock(hClipboardData);
			CloseClipboard();
		}
	}

	void setClipboardText2(wchar_t* wText)
	{
		HGLOBAL hdst;
		LPWSTR dst;
		if (wText != NULL)
		{
			size_t len = wcslen(wText);
			hdst = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (len + 1) * sizeof(wchar_t));
			dst = (LPWSTR)GlobalLock(hdst);
			memcpy(dst, wText, len * sizeof(wchar_t));
			dst[len] = 0;
			GlobalUnlock(hdst);
		}
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			if (wText != NULL)
			{
				SetClipboardData(CF_UNICODETEXT, hdst);
			}
			CloseClipboard();
		}
		Sleep(25);
	}
};
