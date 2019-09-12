#pragma once
#include <xstring>

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

	void createConsole();
	void onCtrlCSignal();
	virtual void println(const char* string);
	virtual void println(const std::string& message) const;
	void print(const std::string& message) const;
	void info(const char* str);
	void infof(const char* str,...);
	void errorf(const char* str,...);
	void error(const char* str);
	void error(const char* str, LogMessageType logMessageType);
};
