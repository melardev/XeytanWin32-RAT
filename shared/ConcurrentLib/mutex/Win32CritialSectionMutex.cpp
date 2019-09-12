#include "Win32CritialSectionMutex.h"

Win32CritialSectionMutex::Win32CritialSectionMutex()
{
	InitializeCriticalSection(&mCriticalSection);
}

Win32CritialSectionMutex::~Win32CritialSectionMutex()
{
	DeleteCriticalSection(&mCriticalSection);
}

void Win32CritialSectionMutex::lock()
{
	EnterCriticalSection(&mCriticalSection);
}

void Win32CritialSectionMutex::unlock()
{
	LeaveCriticalSection(&mCriticalSection);
}
