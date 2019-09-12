#include "StdCVSignal.h"
#include <mutex>

/// This class should be used only for its self explained purpose: signaling
/// As you see in the wait() method it sleeps until someone calls emit() to wake it up
/// And immediately releases the lock, so the client of this API will never have unique lock
/// to work safely with a shared resource

StdCVSignal::StdCVSignal()
{
}

StdCVSignal::~StdCVSignal()
{
}

bool StdCVSignal::wait()
{
	std::unique_lock<std::mutex> lock(mMutex);
	cv.wait(lock);
	return true;
}

bool StdCVSignal::emit()
{
	std::unique_lock<std::mutex> lock(mMutex);
	cv.notify_one(); // Notify worker
	return true;
}
