#include "GraphicsService.h"

/*

GraphicsService::GraphicsService()
{
}


GraphicsService::~GraphicsService()
{
}

int GraphicsService::getMonitorCount()
{
	// int elapsed = monitorsCounter.getCounter();
	if ((firstMonitorsCheck)) // || (elapsed >= MONITORS_INTERVAL))
	{
		firstMonitorsCheck = false;
		monitorsInfo.clear();
		MonitorInfo mi;

		mi.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
		mi.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
		mi.width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		mi.height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		//FIX RISOLUZIONI SIMILE A 1366x768
		if (((float)mi.width / (float)8) != (mi.width / 8))
		{
			mi.width = (int)((int)((float)mi.width / (float)8) + (float)1) * 8;
		}
		//if (((float)mi.h/(float)8)!=(mi.h/8)){
		//   mi.h=(int)((int)((float)mi.h/(float)8)+(float)1) * 8;
		//}

		monitorsInfo.push_back(mi);
		HDC hdc = GetDC(NULL);
		// EnumDisplayMonitors(hdc, 0, ScreenCaptureNativeMonitorEnumProc, (LPARAM)this);
		ReleaseDC(NULL, hdc);
		//Sistema imagesInfo
		for (std::vector<MonitorInfo>::size_type i = 0; i < monitorsInfo.size(); i++)
		{
			if (i >= screenShotInfo.size())
			{
				ScreenShotInfo ii;
				newScreenShotInfo(&ii, monitorsInfo[i].width, monitorsInfo[i].height);
				screenShotInfo.push_back(ii);
			}
			else
			{
				if ((monitorsInfo[i].width != screenShotInfo[i].w) || (monitorsInfo[i].height != screenShotInfo[i].h))
				{
					termScreenShotInfo(&screenShotInfo[i]);
					newScreenShotInfo(&screenShotInfo[i], monitorsInfo[i].width, monitorsInfo[i].height);
				}
			}
		}
		for (std::vector<ScreenShotInfo>::size_type i = monitorsInfo.size(); i < screenShotInfo.size(); i++)
		{
			termScreenShotInfo(&screenShotInfo[i]);
			screenShotInfo.erase(screenShotInfo.begin() + i);
			i--;
		}
		// monitorsCounter.reset();
	}
	if (monitorsInfo.size() == 1)
	{
		//Non trovato nessun monitor
		return 1;
	}
	else
	{
		return monitorsInfo.size() - 1;
	}
}

BOOL GraphicsService::ScreenCaptureNativeMonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor,
                                                         LPARAM dwData)
{
	GraphicsService* pThis = reinterpret_cast<GraphicsService *>(dwData);
	return pThis->monitorEnumProc(hMonitor, hdcMonitor, lprcMonitor, dwData);
}

BOOL GraphicsService::monitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MonitorInfo mi;
	mi.hMonitor = hMonitor;
	mi.hdcMonitor = hdcMonitor;
	mi.x = lprcMonitor->left;
	mi.y = lprcMonitor->top;
	mi.width = lprcMonitor->right - lprcMonitor->left;
	mi.height = lprcMonitor->bottom - lprcMonitor->top;

	//FIX RISOLUZIONI SIMILE A 1366x768
	if (((float)mi.width / (float)8) != (mi.width / 8))
	{
		mi.width = (int)((int)((float)mi.width / (float)8) + (float)1) * 8;
	}
	//if (((float)mi.h/(float)8)!=(mi.h/8)){
	//   mi.h=(int)((int)((float)mi.h/(float)8)+(float)1) * 8;
	//}

	monitorsInfo.push_back(mi);
	return TRUE;
}

void GraphicsService::newScreenShotInfo(ScreenShotInfo* ii, int w, int h)
{
	ii->w = w;
	ii->h = h;
	ii->data = NULL;
	ii->data161616 = NULL;
	ii->shotID = -1;
	// ii->intervallCounter.reset();
}

void GraphicsService::termScreenShotInfo(ScreenShotInfo* ii)
{
	if (ii->shotID >= 0)
	{
		SelectObject(ii->hdestDC, ii->hbmDIBOLD);
		DeleteObject(ii->hbmDIB);
		DeleteDC(ii->hdestDC);
		ReleaseDC(NULL, ii->hsrcDC);
		ii->data = NULL;
		resetScreenShotData(ii);
		ii->shotID = -1;
	}
}

void GraphicsService::resetScreenShotData(ScreenShotInfo* ii)
{
	if (ii->data161616 != NULL)
	{
		free(ii->data161616);
		ii->data161616 = NULL;
	}
}

MonitorInfo* GraphicsService::getMonitorInfo(int idx)
{
	if ((monitorsInfo.size() == 1) && ((idx == 0) || (idx == 1)))
	{
		//Non trovato nessun monitor
		return &monitorsInfo[0];
	}
	else if ((idx >= 0) && (idx <= monitorsInfo.size() - 1))
	{
		return &monitorsInfo[idx];
	}
	else
	{
		return NULL;
	}
}
*/