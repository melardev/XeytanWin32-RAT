#pragma once

#include <Windows.h>
#include <Shlwapi.h>
#include <vector>

#define TRAYICONID	1
#define SWM_TRAYMSG	WM_APP

class GuiCommunicator;

// Most of the code taken from dwservice
struct AppWindow
{
	int id;
	HWND hwnd;
	HDC hdc;
	COLORREF penColor;
	NOTIFYICONDATAW notifyicon;
	HFONT hFont;
	int penWidth;
};

class GuiMediator
{
public:
	GuiMediator();
	~GuiMediator();
	void setCommunicator(GuiCommunicator* communicator);

	void closeWindowByHandle(HWND hwnd)
	{
		for (unsigned int i = 0; i <= openedWindows.size() - 1; i++)
		{
			if (openedWindows.at(i)->hwnd == hwnd)
			{
				AppWindow* window = openedWindows.at(i);
				openedWindows.erase(openedWindows.begin() + i);
				delete window;
				break;
			}
		}
	}

	AppWindow* getWindowByHandle(HWND hwnd)
	{
		if (openedWindows.size() == 0)
			return nullptr;

		for (unsigned int i = 0; i < openedWindows.size() - 1; i++)
		{
			if (openedWindows.at(i)->hwnd == hwnd)
				return openedWindows.at(i);
		}

		return nullptr;
	}

	/*
	 * AppWindow* getWindowByHandle(HWND hwnd)
	{
		if (openedWindows.size() == 0)
		{
			return NULL;
		}
		for (unsigned int i = 0; i <= openedWindows.size() - 1; i++)
		{
			if (openedWindows.at(i)->hwnd == hwnd)
			{
				return openedWindows.at(i);
			}
		}
		return NULL;
	}
	 */

	AppWindow* getWindowById(int id)
	{
		if (openedWindows.size() == 0)
			return nullptr;

		for (unsigned int i = 0; i < openedWindows.size() - 1; i++)
		{
			if (openedWindows.at(i)->id == id)
				return openedWindows.at(i);
		}

		return nullptr;
	}

	void setWindowTitle(int windowId, wchar_t* title)
	{
		AppWindow* window = getWindowById(windowId);
		SetWindowTextW(window->hwnd, title);
	}

	void hideWindow(int id)
	{
		AppWindow* window = getWindowById(id);
		if (window != NULL)
		{
			ShowWindow(window->hwnd, SW_HIDE);
			UpdateWindow(window->hwnd);
		}
	}

	void windowToForeground(int windowId)
	{
		AppWindow* window = getWindowById(windowId);
		if (window != NULL)
		{
			ShowWindow(window->hwnd, SW_RESTORE);
			SetForegroundWindow(window->hwnd);
			UpdateWindow(window->hwnd);
		}
	}

	void repaintWindow(int windowId, int x, int y, int width, int height)
	{
		AppWindow* window = getWindowById(windowId);
		if (window != NULL)
		{
			RECT rec;
			rec.left = x;
			rec.top = y;
			rec.right = x + width;
			rec.bottom = y + height;
			HWND hwnd = window->hwnd;
			InvalidateRect(hwnd, &rec, FALSE);
		}
	}

	AppWindow* addOpenedWindow(int id, HWND hwnd)
	{
		AppWindow* window = new AppWindow;
		window->id = id;
		window->hwnd = hwnd;
		window->penColor = RGB(0, 0, 0);
		window->penWidth = 1;
		window->hFont = CreateFont(0, 0, 0, 0,
		                           FW_NORMAL,
		                           FALSE, FALSE, FALSE,
		                           DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
		                           VARIABLE_PITCH, TEXT("Arial"));
		openedWindows.push_back(window);
		return window;
	}


	/*/*__declspec(dllexport)*/
	void destroyNotifyIcon(int id)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			if (dwawin->notifyicon.uFlags != 0)
			{
				dwawin->notifyicon.uFlags = 0;
				Shell_NotifyIconW(NIM_DELETE, &dwawin->notifyicon);
			}
		}
	}

	/*__declspec(dllexport)*/
	void updateNotifyIcon(int id, wchar_t* iconPath, wchar_t* toolTip)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			if (dwawin->notifyicon.uFlags != 0)
			{
				HINSTANCE hInstance = GetModuleHandle(NULL);
				dwawin->notifyicon.hIcon = (HICON)LoadImageW(hInstance, iconPath, IMAGE_ICON, 0, 0,
				                                             LR_LOADFROMFILE | LR_DEFAULTSIZE);
				wcscpy(dwawin->notifyicon.szTip, toolTip);
				Shell_NotifyIconW(NIM_MODIFY, &dwawin->notifyicon);
				if (dwawin->notifyicon.hIcon && DestroyIcon(dwawin->notifyicon.hIcon))
					dwawin->notifyicon.hIcon = NULL;
			}
		}
	}

	/*__declspec(dllexport)*/
	void createNotifyIcon(int id, wchar_t* iconPath, wchar_t* toolTip)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			if (dwawin->notifyicon.uFlags == 0)
			{
				ZeroMemory(&dwawin->notifyicon, sizeof(NOTIFYICONDATA));
				ULONGLONG ullVersion = getDllVersion("Shell32.dll");
				if (ullVersion >= MAKEDLLVERULL(6, 0, 6, 0))
					dwawin->notifyicon.cbSize = sizeof(NOTIFYICONDATA);
				else if (ullVersion >= MAKEDLLVERULL(6, 0, 0, 0))
					dwawin->notifyicon.cbSize = NOTIFYICONDATA_V3_SIZE;
				else if (ullVersion >= MAKEDLLVERULL(5, 0, 0, 0))
					dwawin->notifyicon.cbSize = NOTIFYICONDATA_V2_SIZE;
				else
					dwawin->notifyicon.cbSize = NOTIFYICONDATA_V1_SIZE;
				dwawin->notifyicon.uID = TRAYICONID;
				dwawin->notifyicon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
				dwawin->notifyicon.hWnd = dwawin->hwnd;
				dwawin->notifyicon.uCallbackMessage = SWM_TRAYMSG;
				updateNotifyIcon(id, iconPath, toolTip);
				HINSTANCE hInstance = GetModuleHandle(NULL);
				dwawin->notifyicon.hIcon = (HICON)LoadImageW(hInstance, iconPath, IMAGE_ICON, 0, 0,
				                                             LR_LOADFROMFILE | LR_DEFAULTSIZE);
				wcscpy(dwawin->notifyicon.szTip, toolTip);
				Shell_NotifyIconW(NIM_ADD, &dwawin->notifyicon);
				if (dwawin->notifyicon.hIcon && DestroyIcon(dwawin->notifyicon.hIcon))
					dwawin->notifyicon.hIcon = NULL;
			}
		}
	}

	ULONGLONG getDllVersion(LPCTSTR lpszDllName)
	{
		ULONGLONG ullVersion = 0;
		HINSTANCE hinstDll;
		hinstDll = LoadLibrary(lpszDllName);
		if (hinstDll)
		{
			DLLGETVERSIONPROC pDllGetVersion;
			pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");
			if (pDllGetVersion)
			{
				DLLVERSIONINFO dvi;
				HRESULT hr;
				ZeroMemory(&dvi, sizeof(dvi));
				dvi.cbSize = sizeof(dvi);
				hr = (*pDllGetVersion)(&dvi);
				if (SUCCEEDED(hr))
					ullVersion = MAKEDLLVERULL(dvi.dwMajorVersion, dvi.dwMinorVersion, 0, 0);
			}
			FreeLibrary(hinstDll);
		}
		return ullVersion;
	}


	/*/*__declspec(dllexport)*/
	void drawLine(int id, int x1, int y1, int x2, int y2)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			HPEN hPenOld;
			HPEN hLinePen;
			hLinePen = CreatePen(PS_SOLID, dwawin->penWidth, dwawin->penColor);
			hPenOld = (HPEN)SelectObject(dwawin->hdc, hLinePen);

			MoveToEx(dwawin->hdc, x1, y1, NULL);
			LineTo(dwawin->hdc, x2, y2);

			SelectObject(dwawin->hdc, hPenOld);
			DeleteObject(hLinePen);
		}
	}

	/*__declspec(dllexport)*/
	void drawEllipse(int id, int x, int y, int w, int h)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			HPEN hLinePen = CreatePen(PS_SOLID, dwawin->penWidth, dwawin->penColor);
			HPEN hPenOld = (HPEN)SelectObject(dwawin->hdc, hLinePen);
			SetDCBrushColor(dwawin->hdc, dwawin->penColor);
			HGDIOBJ hbr = GetStockObject(NULL_BRUSH);
			HBRUSH hbrOld = (HBRUSH)SelectObject(dwawin->hdc, hbr);
			Ellipse(dwawin->hdc, x, y, x + w, y + h);
			SelectObject(dwawin->hdc, hbrOld);
			SelectObject(dwawin->hdc, hPenOld);
			DeleteObject(hbr);
			DeleteObject(hLinePen);
		}
	}

	/*/*__declspec(dllexport)*/
	void fillEllipse(int id, int x, int y, int w, int h)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			HPEN hLinePen = CreatePen(PS_SOLID, dwawin->penWidth, dwawin->penColor);
			HPEN hPenOld = (HPEN)SelectObject(dwawin->hdc, hLinePen);
			SetDCBrushColor(dwawin->hdc, dwawin->penColor);
			HGDIOBJ hbr = GetStockObject(DC_BRUSH);
			HBRUSH hbrOld = (HBRUSH)SelectObject(dwawin->hdc, hbr);
			Ellipse(dwawin->hdc, x, y, x + w, y + h);
			SelectObject(dwawin->hdc, hbrOld);
			SelectObject(dwawin->hdc, hPenOld);
			DeleteObject(hbr);
			DeleteObject(hLinePen);
		}
	}

	/*__declspec(dllexport)*/
	void drawText(int id, wchar_t* str, int x, int y)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			SetTextColor(dwawin->hdc, dwawin->penColor);
			RECT rec;
			rec.left = x;
			rec.top = y;
			rec.right = 0;
			rec.bottom = 0;
			DrawTextExW(dwawin->hdc, str, -1, &rec, DT_LEFT | DT_NOCLIP, NULL);
		}
	}

	/*__declspec(dllexport)*/
	void drawImageFromFile(int id, wchar_t* fname, int x, int y, int w, int h)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, fname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (hBitmap != NULL)
			{
				HDC hdcMem = CreateCompatibleDC(dwawin->hdc);
				HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);
				BITMAP bitmap;
				GetObject(hBitmap, sizeof(bitmap), &bitmap);
				BitBlt(dwawin->hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
				SelectObject(hdcMem, oldBitmap);
				DeleteDC(hdcMem);
			}
		}
	}

	/*__declspec(dllexport)*/
	void fillRectangle(int id, int x, int y, int w, int h)
	{
		AppWindow* dwawin = getWindowById(id);
		if (dwawin != NULL)
		{
			HBRUSH hbrBkgnd = CreateSolidBrush(dwawin->penColor);
			RECT rec;
			rec.left = x;
			rec.top = y;
			rec.right = x + w;
			rec.bottom = y + h;
			FillRect(dwawin->hdc, &rec, hbrBkgnd);
			DeleteObject(hbrBkgnd);
		}
	}

	/*__declspec(dllexport)*/
	void getImageSize(wchar_t* fname, int* sz)
	{
		HBITMAP hBitmap = (HBITMAP)LoadImageW(NULL, fname, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBitmap != NULL)
		{
			BITMAP bitmap;
			GetObject(hBitmap, sizeof(bitmap), &bitmap);
			sz[0] = bitmap.bmWidth;
			sz[1] = bitmap.bmHeight;
		}
		else
		{
			sz[0] = 0;
			sz[1] = 0;
		}
	}

	/*__declspec(dllexport)*/
	int getTextHeight(int id)
	{
		AppWindow* dwawin = getWindowById(id);
		HWND hwnd = dwawin->hwnd;
		TEXTMETRIC lptm;
		HDC dc = NULL;
		if (dwawin->hdc == NULL)
		{
			dc = GetDC(hwnd);
			SelectObject(dc, dwawin->hFont);
		}
		else
		{
			dc = dwawin->hdc;
		}
		GetTextMetrics(dc, &lptm);
		return lptm.tmHeight;
	}

	/*__declspec(dllexport)*/
	int getTextWidth(int id, wchar_t* str)
	{
		AppWindow* dwawin = getWindowById(id);
		HWND hwnd = dwawin->hwnd;
		RECT rec;
		rec.left = 0;
		rec.top = 0;
		rec.right = 100000;
		rec.bottom = 100000;
		HDC dc = NULL;
		if (dwawin->hdc == NULL)
		{
			dc = GetDC(hwnd);
			SelectObject(dc, dwawin->hFont);
		}
		else
		{
			dc = dwawin->hdc;
		}
		if (DrawTextExW(dc, str, -1, &rec, DT_LEFT | DT_NOCLIP | DT_CALCRECT, NULL) == 0)
		{
			return 0;
		}
		return rec.right - rec.left;
	}

private:
	std::vector<AppWindow*> openedWindows;
};
