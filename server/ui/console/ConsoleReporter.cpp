#include "Console.h"
#include <iostream>


Console::Console()
{
}


Console::~Console()
{
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
