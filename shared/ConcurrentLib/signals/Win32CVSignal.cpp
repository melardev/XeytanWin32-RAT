#include "Win32CVSignal.h"

/// This class should be used only for its self explained purpose: signaling
/// As you see in the wait() method it sleeps until someone calls emit() to wake it up
/// And immediately releases the lock, so the client of this API will never have unique lock
/// to work safely with a shared resource
Win32CVSignal::Win32CVSignal(const Win32CritialSectionMutex mu): mMutex(mu)
{
	InitializeConditionVariable(&mConditionVariable);
}


Win32CVSignal::~Win32CVSignal()
{
}

bool Win32CVSignal::wait()
{
	mMutex.lock();
	SleepConditionVariableCS(&mConditionVariable, &mMutex.mCriticalSection, INFINITE);
	mMutex.unlock();
	return true;
}

bool Win32CVSignal::emit()
{
	WakeConditionVariable(&mConditionVariable); // notify one
	// WakeAllConditionVariable(&mConditionVariable); // notify all
	return true;
}
