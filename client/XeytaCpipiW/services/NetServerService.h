#pragma once
/*
#include <vector>

// #include <boost/uuid/uuid.hpp>            // uuid class
// #include <boost/uuid/uuid_generators.hpp> // generators
// #include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

#include "../concurrent/core/Thread.h"
#include "NetClientService.h"

#include "LoggingService.h"
#include "../concurrent/events/net/NetEvent.h"
#include "UuidService.h"


class Mutex;
class NetClientService;

class NetServerService : public NetService
{
public:
	NetServerService();
	~NetServerService();
	void startSendPacketsLoop();
	void start();
	SOCKET createTCPServer(unsigned short port);

	// NetServerService(int port);

	void listenForClients();
	NetClientService* getClientByUuid(UUID* uuid);
	void loopPacketsQueue();
	void sendPacketAsync(Packet* packet);
private:
	std::vector<std::shared_ptr<NetClientService>> clients;
	// std::mutex clientsMutex;
	Mutex* clientsMutex;
	LoggingService* logger;
	int port;
	SOCKET serverSocket;
	// boost::uuids::random_generator uuidGenerator;
	ADDRINFOA* addr;
	hostent* addrResolved;

	std::queue<NetEvent> netEventsToBeProcessed;
	std::queue<Packet*> packetsToBeSent;
	Thread* packetSendThread;
	UuidService* uuidService;
	bool running;
};

*/