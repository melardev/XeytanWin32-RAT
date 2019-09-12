#pragma once
#include <WinSock2.h>
// #pragma comment(lib, "ws2_32.lib")

struct NetError;

class BaseNetService
{
public:
	static void initWinSock2Subsystem();

	BaseNetService(SOCKET sock);
	BaseNetService();
	virtual ~BaseNetService();

	NetError* getErrorOnSend();

	void getErrorOnConnect(NetError& err) const;

	NetError* getErrorOnReceive();

	void connectToHostname(char* hostname, int port);

	void connectToIpAddress(const char* ipAddress, int port) const;

	char* resolveHostName(char* hostname);


	int sendMessage(char* message);

	int receivePacket(char* messageBuffer);

	NetError* getErrorOnSocketCreate();
	void setSocketDescriptor(SOCKET socket);
protected:
	SOCKET socketDescriptor;
};
