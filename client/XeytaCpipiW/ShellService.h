#pragma once
#include <string>
#include "../../../Win32/ConcurrentLib/core/Mutex.h"

class Application;

class ShellService
{
public:
	ShellService(Application* app);

	bool launch();
	bool isRunning() const;
	void writeIntoProcess(const std::string& command);
	void writeIntoProcess(const char* command, size_t commandLen);
	void stop();
	int readFromProcess();

public:
	bool running;
	

private:
	
	HANDLE hProcRead;
	HANDLE hProcWrite;
	HANDLE hThread;
	HANDLE hProcess;
	Application* app;
	HANDLE hReadProcThread;
	char processOutBuffer[1024];
	
};

