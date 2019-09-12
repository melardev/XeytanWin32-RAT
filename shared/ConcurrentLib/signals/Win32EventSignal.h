#pragma once
#include "ISignal.h"
#include <string>

typedef void *HANDLE;
class Win32EventSignal : public ISignal
{
public:
	Win32EventSignal();
	Win32EventSignal(const std::string& eventName);
	~Win32EventSignal();
	virtual bool wait() override;
	virtual bool emit() override;
	HANDLE hEvent;
};

