#include "Win32EventSignal.h"
#include <Windows.h>
#include <string>


Win32EventSignal::Win32EventSignal()
{
	hEvent = CreateEventA(NULL, TRUE, FALSE, "Win32EventSignal");
}

Win32EventSignal::Win32EventSignal(const std::string& eventName)
{
	hEvent = CreateEventA(NULL, TRUE, FALSE /* Initial State Non Signaled */,
	                      eventName.c_str());
}


Win32EventSignal::~Win32EventSignal()
{
}

bool Win32EventSignal::wait()
{
	const DWORD dwWaitResult = WaitForSingleObject(hEvent, INFINITE);
	return dwWaitResult == WAIT_OBJECT_0;
}

bool Win32EventSignal::emit()
{
	// On success return value != 0
	return SetEvent(hEvent) != 0;
}
