#pragma once

#include <Windows.h>

enum LogMessageType
{
	Error,
	Debug,
	Info,
	Trace
};
class Console
{
public:
	Console();
	~Console();
	void info(const char* str);
	void infof(const char* str,...);
	void errorf(const char* str,...);
	void error(const char* str);
	void error(const char* str, LogMessageType logMessageType);
	
};

