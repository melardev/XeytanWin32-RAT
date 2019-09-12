#include "ShellService.h"
#include <Windows.h>
#include <process.h>
#include <tchar.h>
#include <strsafe.h>
#include "Application.h"

/* Typedefs */
typedef HANDLE PIPE;


ShellService::ShellService(Application* app) : app(app)
{
}

bool ShellService::launch()
{
	//Process Setup
	SECURITY_ATTRIBUTES secAttrs;
	STARTUPINFO sInfo = {0};
	PROCESS_INFORMATION pInfo = {0};
	PIPE hProcInRead, hProcInWrite, hProcOutRead, hProcOutWrite;
	TCHAR cmdPath[MAX_PATH];

	secAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttrs.bInheritHandle = TRUE;
	secAttrs.lpSecurityDescriptor = NULL;


	if (!CreatePipe(&hProcInRead, &hProcInWrite, &secAttrs, 0))
	{
		// ErrorExit(TEXT("hProcIn CreatePipe"));
		return FALSE;
	}

	// Ensure the write handle to the pipe for STDIN is not inherited. 
	// The Child Process does not have to be able
	// to WRITE to input, it only needs to READ from the input.
	if (!SetHandleInformation(hProcInWrite, HANDLE_FLAG_INHERIT, 0))
	{
		// ErrorExit(TEXT("StdIn SetHandleInformation"));
		return FALSE;
	}

	if (!CreatePipe(&hProcOutRead, &hProcOutWrite, &secAttrs, 0))
	{
		// ErrorExit(TEXT("hProcOut CreatePipe"));
		return FALSE;
	}

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	// The Child Process does not have to be able
	// to READ from output, it only needs to WRITE to the output.
	if (!SetHandleInformation(hProcOutRead, HANDLE_FLAG_INHERIT, 0))
	{
		// ErrorExit(TEXT("StdIn SetHandleInformation"));
		return FALSE;
	}

	GetEnvironmentVariable(_T("ComSpec"), cmdPath, sizeof(cmdPath));

	sInfo.cb = sizeof(STARTUPINFO);
	sInfo.wShowWindow = SW_HIDE;

	// Setup Redirection
	sInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	sInfo.hStdInput = hProcInRead;
	// process will be writing output into the writing side of the pipe we will be reading from that pipe using the reading side.
	sInfo.hStdOutput = hProcOutWrite;
	// process will be writing errors into the writing side of the pipe we will be reading from that pipe using the reading side.
	sInfo.hStdError = hProcOutWrite;

	CreateProcess(NULL,
	              cmdPath,
	              &secAttrs,
	              &secAttrs,
	              TRUE, // Inherit Handles(Including PIPES)
	              0,
	              NULL,
	              NULL,
	              &sInfo, &pInfo);

	hProcRead = hProcOutRead;
	hProcWrite = hProcInWrite;
	hThread = pInfo.hThread;
	hProcess = pInfo.hProcess;

	running = true;
	hReadProcThread = reinterpret_cast<HANDLE>(
		_beginthreadex(0, 0,
		               [](void* arg) -> unsigned int
		               {
			               ShellService* shellService = (ShellService*)arg;
			               return shellService->readFromProcess();
		               }, this, 0, NULL));

	return TRUE;
}

bool ShellService::isRunning() const
{
	return running;
}

void ShellService::writeIntoProcess(const char* command, size_t commandLen)
{
	if (!running)
		return;

	DWORD dwWrite, dwTotalWritten = 0;
	BOOL bSuccess = FALSE;

	while (dwTotalWritten < commandLen)
	{
		bSuccess = WriteFile(hProcWrite, command + dwTotalWritten, commandLen - dwTotalWritten, &dwWrite, NULL);

		if (!bSuccess || _strcmpi(command, "exit\n") == 0)
		{
			stop();
			app->onShellClosed();
			return;
		}
		dwTotalWritten += dwWrite;
	}


#ifdef _CONSOLE
	const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	dwTotalWritten = 0;

	if (hStdOut != INVALID_HANDLE_VALUE)
	{
		while (dwTotalWritten < commandLen)
		{
			bSuccess = WriteFile(hStdOut, command + dwTotalWritten, commandLen - dwTotalWritten, &dwWrite, NULL);
			if (!bSuccess)break;
			dwTotalWritten += dwWrite;
		}
	}
#endif
}

void ShellService::stop()
{
	DisconnectNamedPipe(hProcRead); // Needed otherwise CloseHandle hProcRead hangs
	CloseHandle(hProcRead);
	CloseHandle(hProcWrite);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	running = false;
	// TerminateProcess(state->hProcess, 0);


	WaitForSingleObject(hReadProcThread, INFINITE);
}

int ShellService::readFromProcess()
{
	if (!running)
		return 0;

	DWORD dwWritten, dwProcRead;

	BOOL bSuccess = FALSE;

	while (running)
	{
		bSuccess = ReadFile(hProcRead, processOutBuffer, 1024, &dwProcRead, NULL);
		if (!bSuccess || dwProcRead == 0) break;

		app->onShellOutput(processOutBuffer, dwProcRead);

		if (!bSuccess) break;
	}

	return 0;
}

void ShellService::writeIntoProcess(const std::string& command)
{
	writeIntoProcess(command.c_str(), command.size());
}


void ErrorExit(const TCHAR* lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
	                                  (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(
		                                  TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
	                LocalSize(lpDisplayBuf) / sizeof(TCHAR),
	                TEXT("%s failed with error %d: %s"),
	                lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(1);
}
