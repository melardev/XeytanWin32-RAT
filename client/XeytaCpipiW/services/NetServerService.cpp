#include "NetServerService.h"
/*
#include "../concurrent/core/Mutex.h"
#include "../concurrent/events/net/NewConnectionEvent.h"



DWORD WINAPI listenPacketsToBeSentThreadFunc(LPVOID threadArg)
{
	auto netServerService = static_cast<NetServerService*>(threadArg);
	netServerService->loopPacketsQueue();
	return 0;
}

DWORD WINAPI clientThreadFunc(LPVOID threadArg)
{
	NetClientService* arg = (NetClientService*)threadArg;
	arg->Interact();
	return 0;
}

NetServerService::NetServerService()
{
	clientsMutex = new Mutex("clientsMutex");
}


NetServerService::~NetServerService()
{
	// TODO: work this better, there may be race issues
	UUID* uuid;
	for (auto const client : clients)
	{
		uuid = client->getUuid();
		uuidService->freeUuid(uuid);
	}
}

void NetServerService::startSendPacketsLoop()
{
	packetSendThread = new Thread((LPTHREAD_START_ROUTINE)listenPacketsToBeSentThreadFunc, (PVOID)this, 0, 0, TRUE);
	packetSendThread->run();
}

void NetServerService::start()
{
	startSendPacketsLoop();
	listenForClients();
}


SOCKET NetServerService::createTCPServer(unsigned short port)
{
	int iResult;

	ADDRINFOA addr;
	ADDRINFOA* ultimate;
	// hostent* addrResolved;
	SOCKET s;

	// setRunning(true);


	ZeroMemory(&addr, sizeof(ADDRINFOA));

	addr.ai_family = AF_INET;
	addr.ai_socktype = SOCK_STREAM;
	addr.ai_protocol = IPPROTO_IP;
	addr.ai_flags = AI_PASSIVE; // msdn : The clientSocket address will be used in a call to the bind function.

	// iResult = getaddrinfo(NULL, (std::to_string(port)).c_str(), &addr, &ultimate);
	iResult = 0;
	if (iResult != 0)
	{
		// printf("Failure on getaddrinfo with error %d\n", iResult);
		WSACleanup();
		return 1;
	}
	ultimate = new ADDRINFOA;
	s = socket(ultimate->ai_family, ultimate->ai_socktype, ultimate->ai_protocol);
	if (s == INVALID_SOCKET)
	{
		// printf("Failure in clientSocket() %d\n", WSAGetLastError());
		// freeaddrinfo(ultimate);
		WSACleanup();
		return 1;
	}
	int yes = 1;
	iResult = setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));
	if (iResult != 0)
	{
		// printf("error on setsockopt() %d", iResult);
		// freeaddrinfo(ultimate);
		closesocket(s);
		WSACleanup();
		return 1;
	}

	//I should to find use of gethostbyname
	//addrResolved = gethostbyname(hostname.c_str());
	//addrResolved->h_name;

	//iResult = ::bind(s, (sockaddr*) &addr, sizeof(SOCKADDR)); WRONG , FROM WHERE I TOOK THAT 
	iResult = ::bind(s, ultimate->ai_addr, (int)ultimate->ai_addrlen);

	if (iResult == SOCKET_ERROR)
	{
		// printf("error on bind() %d", WSAGetLastError());
		// freeaddrinfo(ultimate);
		closesocket(s);
		WSACleanup();
		return 1;
	}

	// freeaddrinfo(ultimate);
	listen(s, SOMAXCONN);
	return s;
}

void NetServerService::listenForClients()
{
	// Either this is tarted from a background thread, or this should spawn a new thread
	WSAData wsaData;
	logger->infof("[+] Waiting for client for incomming clients at %1 ...", port);

	int iResult;
	if (wsaData.szSystemStatus != "Running")
	{
		if ((iResult = WSAStartup(MAKEWORD(2, 2), &wsaData) != 0))
		{
			logger->errorf("Failure on wsastartup with error %d\n", iResult);
			return;
		}
	}

	if ((serverSocket = createTCPServer(port)) != INVALID_SOCKET)
	{
		logger->info("[*] Waiting for clients , please wait before executing commands ...");
	}

	while (true)
	{
		// SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&addr, sizeof(addr));
		SOCKET clientSocket = 0;
		if (clientSocket == 0)
			logger->error("");
		else
		{
			// boost::uuids::uuid uuid = uuidGenerator();
			// std::lock_guard<std::mutex> lock(clientsMutex);
			clientsMutex->lock();
			std::shared_ptr<NetClientService> client(new NetClientService);
			clients.push_back(client);
			clientsMutex->unlock();
			auto thread = new Thread((LPTHREAD_START_ROUTINE)clientThreadFunc,
			                         (void*)&client, 0, 0,FALSE);
			client->setThread(thread);
			thread->run();

			NewConnectionEvent event(client);
			netEventsToBeProcessed.emplace(event);
		}
	}
}

NetClientService* NetServerService::getClientByUuid(UUID* uuid)
{
	return nullptr;
}

void NetServerService::loopPacketsQueue()
{
	Packet* packet;
	UUID* uuid;
	NetClientService* client;
	while (running)
	{
		if (packetsToBeSent.size() > 0)
		{
			// TODO: how to handle locking?
			clientsMutex->lock();
			packet = packetsToBeSent.front();
			uuid = packet->getDestination();
			client = getClientByUuid(uuid);
			client->sendPacket(packet);
			clientsMutex->unlock();
		}
	}
}

void NetServerService::sendPacketAsync(Packet* packet)
{
	// TODO: there may be hang issues if this is called from the main thread and lock
	// takes too long too own it
	clientsMutex->lock();
	packetsToBeSent.push(packet);
	clientsMutex->unlock();
}

/*
void NetServerService::OnClientDisconnected(boost::uuids::uuid uuid)
{
	std::lock_guard<std::mutex> lock(clientsMutex);
	clients[uuid]->packetDispatcher.Clear();
	clients[uuid]->CloseAllConnections();
	closesocket(clients[uuid]->clientSocket);
}
*/
