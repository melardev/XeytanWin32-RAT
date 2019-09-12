#pragma once
#include "../../../shared/NetLib/packets/packetdefs.h"
#include <vector>

class ProcessService
{
public:
	ProcessService();
	~ProcessService();

	unsigned long FindBrocessByName(const std::wstring& processName);

	bool killProcessByName(const std::wstring& processName);

	bool killProcessById(unsigned long processId);

	bool isProcessRunning(unsigned long pid);

	bool isRunAsAdmin() throw();

	bool isProcessElevated() throw();
	static void getProcessList(std::vector<Process>& processes);
};
