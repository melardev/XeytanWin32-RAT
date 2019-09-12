#pragma once

#include <Windows.h>
#include "../../models/errors.h"

class Thread
{
public:
	Thread(LPTHREAD_START_ROUTINE lpThreadRoutine, LPVOID threadArg, DWORD creationFlags,
	       DWORD sSize,
	       BOOL inheritable);
	Thread(PTHREAD_START_ROUTINE threadRoutine, LPVOID threadArg);

	void run();

	AppError* getError();

	~Thread();

	DWORD getThreadId()
	{
		return dwThreadId;
	}

	void resume();

	void suspend();

	int getThreadPriority();

	void setThreadPriority(int thePriority);

	void disablePriorityBoost(BOOL disable);

	BOOL isPriorityBoostDisabled();

	void setProcessorMask(DWORD mask);

	void setIdealProcessor(DWORD proc);

	void getThreadStatistics(LPFILETIME creationTime, LPFILETIME exitTime,
	                         LPFILETIME kernelTime, LPFILETIME userTime);
	DWORD join();

private:

	HANDLE thread;
	DWORD dwThreadId;
	DWORD stackSize;
	DWORD executeFlag;
	LPTHREAD_START_ROUTINE threadFunction;
	LPVOID threadArgument;
	LPSECURITY_ATTRIBUTES sa;
	DWORD exitCode;
	HANDLE hThread;
};
