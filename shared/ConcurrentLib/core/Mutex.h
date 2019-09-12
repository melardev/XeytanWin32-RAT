#pragma once

typedef void *HANDLE;
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
