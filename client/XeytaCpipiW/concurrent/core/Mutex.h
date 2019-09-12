#pragma once
#include <Windows.h>

class Mutex
{
public:

	void lock();
	void unlock();
	Mutex(const char* name);
	
	~Mutex();

private:
	HANDLE hMutex;
};
