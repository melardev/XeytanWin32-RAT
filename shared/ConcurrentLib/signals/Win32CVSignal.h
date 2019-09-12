#pragma once
#include "ISignal.h"
#include "../mutex/Win32CritialSectionMutex.h"

class Win32CVSignal : public ISignal
{
public:
	
	Win32CVSignal(Win32CritialSectionMutex mu);
	~Win32CVSignal();
	virtual bool wait() override;
	virtual bool emit() override;
	CONDITION_VARIABLE mConditionVariable;
	CRITICAL_SECTION mLock;
	Win32CritialSectionMutex mMutex;
};

