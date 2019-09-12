#pragma once
#include <Windows.h>
#include <string>
#include "../../models/errors.h"


class Semaphore
{
public:
	long initialCount;
	long maxCount;
	BOOL canInherit;

	Semaphore(std::string name, long initialCount, long maxCount, BOOL canInherit)
	{
		sa = (LPSECURITY_ATTRIBUTES)HeapAlloc(GetProcessHeap(), 0, sizeof(SECURITY_ATTRIBUTES));
		sa->nLength = sizeof(SECURITY_ATTRIBUTES);
		sa->lpSecurityDescriptor = NULL; //no restrictions
		sa->bInheritHandle = canInherit;
		
		hSemaphore = CreateSemaphoreA(sa, initialCount, maxCount, name.c_str());
		if (hSemaphore == NULL)
		{
			int errCode = 0;
			std::string header = "CreateSemaphore failed";
			// getErrorDetail(errCode, header);
			// myException excp(errCode, header);
			// throw excp;
			return;
		}

		this->initialCount = initialCount;
		this->maxCount = initialCount;
		this->canInherit = canInherit;
		if (name.compare(""))
			this->name = name;
	
	}

	~Semaphore()
	{
		if (sa)
			HeapFree(GetProcessHeap(), 0, sa);

		if (hSemaphore)
		{
			close();
		}
	}


	BOOL lock(BOOL wait)
	{
		if (wait == TRUE)
		{
			if (WaitForSingleObject(hSemaphore, INFINITE) == WAIT_FAILED)
			{
				// Error
			}
			return TRUE;
		}
		else
		{
			DWORD retVal = WaitForSingleObject(hSemaphore, 0);

			if (retVal == WAIT_TIMEOUT) return FALSE;
			if (retVal == WAIT_OBJECT_0) return TRUE;

			if (retVal == WAIT_FAILED)
			{
				// Error
			}
		}

		return FALSE;
	}

	void unlock()
	{
		if (!ReleaseSemaphore(hSemaphore, 1, NULL))
		{
			// Error
		}
	}

	void close()
	{
		if (!CloseHandle(hSemaphore))
		{
			// Error
		}
		hSemaphore = NULL;
	}

	AppError* getError(int& errCode, std::string& header)
	{
		char buf[256];
		LPVOID lpDetailMsgBuf;
		UINT errorCode;

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		              NULL, errorCode = GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		              (LPTSTR)&lpDetailMsgBuf, 0, NULL);

		memset(buf, 0, sizeof(buf));
		wsprintf(buf, ": %s", (LPTSTR)lpDetailMsgBuf);
		LocalFree(lpDetailMsgBuf);

		header.append(std::string(buf));
		errCode = errorCode;
	}

private:
	HANDLE hSemaphore;
	LPSECURITY_ATTRIBUTES sa;
	std::string name;
};
