#include "Thread.h"
#include "../models/errors.h"

Thread::Thread(LPTHREAD_START_ROUTINE lpThreadRoutine, LPVOID threadArg, DWORD creationFlags, DWORD sSize,
               BOOL inheritable)
{
	sa = (LPSECURITY_ATTRIBUTES)HeapAlloc(GetProcessHeap(), 0, sizeof(SECURITY_ATTRIBUTES));
	sa->nLength = sizeof(SECURITY_ATTRIBUTES);
	sa->lpSecurityDescriptor = NULL;
	sa->bInheritHandle = inheritable;

	threadFunction = lpThreadRoutine;
	threadArgument = threadArg;

	executeFlag = creationFlags;
	stackSize = sSize;
}

Thread::Thread(PTHREAD_START_ROUTINE lpThreadRoutine, PVOID threadArg)
	: Thread(lpThreadRoutine, threadArg, 0, 0, TRUE)
{
}

void Thread::run()
{
	hThread = CreateThread(sa, stackSize, threadFunction, threadArgument,
	                       executeFlag, &dwThreadId);
	if (hThread == NULL)
	{
		AppError* err = getError();
		throw err;
	}
}

AppError* Thread::getError()
{
	AppError* error = (AppError*)HeapAlloc(GetProcessHeap(), 0, sizeof(AppError));
	char buf[256];
	LPVOID lpDetailMsgBuf;
	UINT errorCode;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	              NULL, errorCode = GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	              (LPTSTR)&lpDetailMsgBuf, 0, NULL);

	memset(buf, 0, 255);
	wsprintfA(buf, ": %s", (LPTSTR)lpDetailMsgBuf);
	LocalFree(lpDetailMsgBuf);

	error->messageChar = buf;
	error->errorCode = errorCode;
	return error;
}

Thread::~Thread()
{
	if (sa)
	{
		HeapFree(GetProcessHeap(), 0, sa);
		sa = NULL;
	}
}

void Thread::resume()
{
	if (ResumeThread(thread) == 0xFFFFFFFF)
	{
		AppError* err = getError();
		throw err;
	}
}

void Thread::suspend()
{
	if (SuspendThread(thread) == 0xFFFFFFFF)
	{
		AppError* err = getError();
		throw err;
	}
}

int Thread::getThreadPriority()
{
	int retVal;
	if ((retVal = GetThreadPriority(thread)) == THREAD_PRIORITY_ERROR_RETURN)
	{
		AppError* err = getError();
		throw err;
	}
	return retVal;
}

void Thread::setThreadPriority(int thePriority)
{
	if (!SetThreadPriority(thread, thePriority))
	{
		int errCode = 0;
		AppError* err = getError();
		throw err;
	}
}

void Thread::disablePriorityBoost(BOOL disable)
{
	if (!SetThreadPriorityBoost(thread, disable))
	{
		int errCode = 0;
		AppError* err = getError();
		throw err;
	}
}

BOOL Thread::isPriorityBoostDisabled()
{
	BOOL boosted;

	if (!GetThreadPriorityBoost(thread, &boosted))
	{
		int errCode = 0;
		AppError* err = getError();
		throw err;
	}

	return boosted;
}

void Thread::setProcessorMask(DWORD mask)
{
	if (!SetThreadAffinityMask(thread, mask))
	{
		int errCode = 0;
		AppError* err = getError();
		throw err;
	}
}

void Thread::setIdealProcessor(DWORD proc)
{
	if (SetThreadIdealProcessor(thread, proc) == -1)
	{
		int errCode = 0;
		AppError* err = getError();
		throw err;
	}
}

void Thread::getThreadStatistics(LPFILETIME creationTime, LPFILETIME exitTime, LPFILETIME kernelTime,
                                 LPFILETIME userTime)
{
	if (!GetThreadTimes(thread, creationTime, exitTime, kernelTime, userTime))
	{
		AppError* err = getError();
		throw err;
	}
}

void Thread::join()
{
	WaitForSingleObject(hThread, INFINITE);
}
