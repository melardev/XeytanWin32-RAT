#pragma once
#include <Windows.h>

class Win32CritialSectionMutex
{
public:
	Win32CritialSectionMutex();
	~Win32CritialSectionMutex();
	void lock();
	void unlock();
	CRITICAL_SECTION mCriticalSection;
};

