#pragma once
/*
#include <vector>
#include <Windows.h>

#define MONITORS_INTERVAL 3000

struct MonitorInfo
{
	HMONITOR hMonitor;
	HDC hdcMonitor;
	int x, y, width, height;
};

struct ScreenShotInfo
{
	HDC hsrcDC;
	HDC hdestDC;
	BITMAPINFO bitmapInfo;
	HANDLE hbmDIB;
	HANDLE hbmDIBOLD;
	int w;
	int h;
	unsigned char* data;
	short* data161616;
	long shotID;
};

class GraphicsService
{
public:
	GraphicsService();
	~GraphicsService();

	int getMonitorCount();


	BOOL CALLBACK ScreenCaptureNativeMonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor,
	                                                 LPARAM dwData);

	BOOL CALLBACK monitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);

	void newScreenShotInfo(ScreenShotInfo* ii, int w, int h);

	void termScreenShotInfo(ScreenShotInfo* ii);

	void resetScreenShotData(ScreenShotInfo* ii);

	MonitorInfo* getMonitorInfo(int idx);
private:

	std::vector<MonitorInfo> monitorsInfo;
	bool firstMonitorsCheck;
	std::vector<ScreenShotInfo> screenShotInfo;
};
*/