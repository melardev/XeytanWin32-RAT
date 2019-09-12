#include "Console.h"
#include <Windows.h>
#include <iostream>
#include <fcntl.h>
#include <io.h>
Console* console;

BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		console->onCtrlCSignal();
		return TRUE;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		// Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		return TRUE;

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:

		printf("Ctrl-Break event\n\n");
		return TRUE;

	case CTRL_LOGOFF_EVENT:

		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:

		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}


void Console::createConsole()
{
	console = this;
	AllocConsole();
	SetConsoleTitleA("Server");
	typedef struct
	{
		char* _ptr;
		int _cnt;
		char* _base;
		int _flag;
		int _file;
		int _charbuf;
		int _bufsiz;
		char* _tmpfname;
	} FILE_COMPLETE;
	*(FILE_COMPLETE*)stdout = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT),
	                                                   "w");
	*(FILE_COMPLETE*)stderr = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT),
	                                                   "w");
	*(FILE_COMPLETE*)stdin = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT),
	                                                  "r");
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
	};
}

void Console::onCtrlCSignal()
{
}

Console::Console()
{
	createConsole();
}


Console::~Console()
{
}

void Console::println(const char* string)
{
}

void Console::println(const std::string& message) const
{
	std::cout << message.c_str() << std::endl;
	std::cout.flush();
}

void Console::print(const std::string& message) const
{
	std::cout << message.c_str();
}

void Console::info(const char* str)
{
}

void Console::infof(const char* str, ...)
{
}

void Console::errorf(const char* str, ...)
{
}

void Console::error(const char* str)
{
}


void Console::error(const char* str, LogMessageType logMessageType)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (logMessageType)
	{
	case 1:
		SetConsoleTextAttribute(hConsole, 10);
		std::cout << str << std::endl;
		SetConsoleTextAttribute(hConsole, 7);
		break;
	case Error:
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		std::cout << "[ERROR] " << str << std::endl;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case 3:
		//SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
		std::cout << str;
		//SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
}
