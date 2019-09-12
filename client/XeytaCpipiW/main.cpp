#include "Application.h"

#pragma comment (lib, "Wbemuuid.lib")
// #pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000

Application app;

void testSome()
{
}

#ifdef _CONSOLE
int main()
#else
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
#endif
{
	// Main Thread: gui -> mediator -> queues Event to Communicator Queue
	// Background Thread: Communicator takes event dispatches it to App
	// App takes event and submits task to ThreadPool to free the Queue Thread as 
	// quick as possible
	testSome();
	return app.run();
}
