#pragma once
/*
#include "GraphicsService.h"

class MouseService
{
public:
	MouseService();
	~MouseService();

	void getMousePosition(int* pos)
	{
		POINT pt;
		GetCursorPos(&pt);
		pos[0] = pt.x;
		pos[1] = pt.y;
	}

	void sendMouseInput(int monitor, int x, int y, int button, int wheel, bool ctrl, bool alt, bool shift) {
		/* ctrlaltshift(ctrl, alt, shift);
		int appx = 0;
		int appy = 0;
		int mouseData = 0;
		DWORD dwFlags = 0;
		if ((x != -1) && (y != -1)) {
			int mx = x;
			int my = y;
			MonitorInfo* mi = getMonitorInfo(monitor);
			if (mi != NULL) {
				mx = mx + mi->x;
				my = my + mi->y;
			}
			dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
			UINT16 desktopWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			UINT16 desktopHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

			int desktopOffsetX = GetSystemMetrics(SM_XVIRTUALSCREEN);
			int desktopOffsetY = GetSystemMetrics(SM_YVIRTUALSCREEN);
			appx = (int)((mx - desktopOffsetX) * 65535 / (desktopWidth));
			appy = (int)((my - desktopOffsetY) * 65535 / (desktopHeight));
		}
		if (button == 64) { //CLICK
			sendInputMouse(appx, appy, dwFlags | MOUSEEVENTF_LEFTDOWN, mouseData);
			Sleep(10);
			sendInputMouse(appx, appy, dwFlags | MOUSEEVENTF_LEFTUP, mouseData);
		}
		else if (button == 128) { //DBLCLICK
			sendInputMouse(appx, appy, dwFlags | MOUSEEVENTF_LEFTDOWN, mouseData);
			Sleep(10);
			sendInputMouse(appx, appy, dwFlags | MOUSEEVENTF_LEFTUP, mouseData);
			Sleep(10);
			sendInputMouse(appx, appy, dwFlags | MOUSEEVENTF_LEFTDOWN, mouseData);
			Sleep(10);
			sendInputMouse(appx, appy, dwFlags | MOUSEEVENTF_LEFTUP, mouseData);
		}
		else {
			if (button != -1) {
				if ((button & 1) && (!mousebtn1Down)) {
					dwFlags |= MOUSEEVENTF_LEFTDOWN;
					mousebtn1Down = true;
				}
				else if (mousebtn1Down) {
					dwFlags |= MOUSEEVENTF_LEFTUP;
					mousebtn1Down = false;
				}

				if ((button & 2) && (!mousebtn2Down)) {
					dwFlags |= MOUSEEVENTF_RIGHTDOWN;
					mousebtn2Down = true;
				}
				else if (mousebtn2Down) {
					dwFlags |= MOUSEEVENTF_RIGHTUP;
					mousebtn2Down = false;
				}

				if ((button & 4) && (!mousebtn3Down)) {
					dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
					mousebtn3Down = true;
				}
				else if (mousebtn3Down) {
					dwFlags |= MOUSEEVENTF_MIDDLEUP;
					mousebtn3Down = false;
				}
			}
			if (wheel != 0) {
				dwFlags |= MOUSEEVENTF_WHEEL;
				mouseData = wheel * 120;
			}
			sendInputMouse(appx, appy, dwFlags, mouseData);
			fastCounter.reset();
		}
*/

/*
	}

	void sendInputMouse(int x, int y, DWORD dwFlags, int mouseData) {
		INPUT inputs;
		inputs.type = INPUT_MOUSE;
		inputs.mi.dx = x;
		inputs.mi.dy = y;
		inputs.mi.dwFlags = dwFlags;
		inputs.mi.time = 0;
		inputs.mi.dwExtraInfo = 0;
		inputs.mi.mouseData = mouseData;
		SendInput(1, &inputs, sizeof(INPUT));
	}

private:
	bool mousebtn1Down;
	bool mousebtn2Down;
	bool mousebtn3Down;

	HCURSOR cursorHandle;
	long cursorID;
	int cursorX;
	int cursorY;
	int cursoroffsetX;
	int cursoroffsetY;
	int cursorW;
	int cursorH;
};
*/