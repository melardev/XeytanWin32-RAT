#pragma once
class LoggingService
{
public:
	LoggingService();
	~LoggingService();
	void info(const char* str);
	void infof(const char* str, ... );
	void errorf(const char* str, ...);
	void error(const char* str);
};

