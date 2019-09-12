#include "Mutex.h"
#include <Windows.h>

// https://docs.microsoft.com/en-us/windows/desktop/sync/using-mutex-objects
void Mutex::lock()
{
	DWORD dwWaitResult = WaitForSingleObject(hMutex, INFINITE);
}

void Mutex::unlock()
{
	DWORD dwResult = ReleaseMutex(hMutex);
	if (!dwResult)
	{
		// Error
	}
}

// TODO: throw exception if error on creating mutex
Mutex::Mutex(const char* name)
{
	hMutex = CreateMutexA(NULL, // default security attributes
		FALSE, // initially not owned
		name); // mutex name

	if (hMutex == NULL)
	{
		GetLastError();
	}
}


Mutex::~Mutex()
{
	if (hMutex != NULL)
		CloseHandle(hMutex);
}
