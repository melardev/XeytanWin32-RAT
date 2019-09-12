#include "BaseNetService.h"
#include <iostream>
#include <WS2tcpip.h>
#include "../models/errors.h"

void BaseNetService::initWinSock2Subsystem()
{
	static WSADATA wsaData;
	std::cout << "" << std::endl;

	if (strcmp(wsaData.szSystemStatus, "Running") != 0)
	{
		const DWORD result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			NetError initializationException{0, "Error: calling WSAStartup()"};
			throw initializationException;
		}
	}
}

BaseNetService::BaseNetService(SOCKET sock) : socketDescriptor(sock)
{
	initWinSock2Subsystem();
}

BaseNetService::BaseNetService()
{
	initWinSock2Subsystem();
}

BaseNetService::~BaseNetService()
{
}

NetError* BaseNetService::getErrorOnSend()
{
	// TODO: Messages Copied, change messages
	NetError* err = (NetError*)GlobalAlloc(0, sizeof NetError);
	err->errorCode = WSAGetLastError();

	if (err->errorCode == WSANOTINITIALISED)
		err->messageChar.append("A successful WSAStartup must occur before using this function.");

	else if (err->errorCode == WSAENETDOWN)
		err->messageChar.append("The network subsystem has failed.");

	else if (err->errorCode == WSAEACCES)
	{
		err->messageChar.append("The requested address is a broadcast address,\n");

		err->messageChar.append("but the appropriate flag was not set. Call setsockopt\n");

		err->messageChar.append("with the SO_BROADCAST parameter to allow the use of the broadcast address.");
	}
	else if (err->errorCode == WSAEINTR)
	{
		err->messageChar.append("A blocking Windows Sockets 1.1 call was canceled\n");

		err->messageChar.append("through WSACancelBlockingCall.");
	}
	else if (err->errorCode == WSAEINPROGRESS)
	{
		err->messageChar.append("A blocking Windows Sockets 1.1 call is in progress,\n");

		err->messageChar.append("or the service provider is still processing a callback function.");
	}
	else if (err->errorCode == WSAEFAULT)
	{
		err->messageChar.append("The buf parameter is not completely contained in a\n");

		err->messageChar.append("valid part of the user address space.");
	}
	else if (err->errorCode == WSAENETRESET)
	{
		err->messageChar.append("The connection has been broken due to the keep-alive\n");

		err->messageChar.append("activity detecting a failure while the operation was in progress.");
	}
	else if (err->errorCode == WSAENOBUFS)
		err->messageChar.append("No buffer space is available.");

	else if (err->errorCode == WSAENOTCONN)
		err->messageChar.append("The socket is not connected.");

	else if (err->errorCode == WSAENOTSOCK)
		err->messageChar.append("The descriptor is not a socket.");

	else if (err->errorCode == WSAEOPNOTSUPP)
	{
		err->messageChar.append("MSG_OOB was specified, but the socket is not stream-style\n");

		err->messageChar.append("such as type SOCK_STREAM, out-of-band data is not supported\n");

		err->messageChar.append("in the communication domain associated with this socket,\n");

		err->messageChar.append("or the socket is unidirectional and supports only receive operations.");
	}
	else if (err->errorCode == WSAESHUTDOWN)
	{
		err->messageChar.append("The socket has been shut down; it is not possible to send\n");

		err->messageChar.append("on a socket after shutdown has been invoked with how set\n");

		err->messageChar.append("to SD_SEND or SD_BOTH.");
	}
	else if (err->errorCode == WSAEWOULDBLOCK)
		err->messageChar.append("The socket is marked as nonblocking and the requested operation would block.\n");

	else if (err->errorCode == WSAEMSGSIZE)
	{
		err->messageChar.append("The socket is message oriented, and the message is larger\n");

		err->messageChar.append("than the maximum supported by the underlying transport.");
	}
	else if (err->errorCode == WSAEHOSTUNREACH)
		err->messageChar.append("The remote host cannot be reached from this host at this time.");

	else if (err->errorCode == WSAEINVAL)
	{
		err->messageChar.append("The socket has not been bound with bind, or an unknown flag\n");

		err->messageChar.append("was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.");
	}
	else if (err->errorCode == WSAECONNABORTED)
	{
		err->messageChar.append("The virtual circuit was terminated due to a time-out or \n");

		err->messageChar.append("other failure. The application should close the socket as it is no longer usable.");
	}
	else if (err->errorCode == WSAECONNRESET)
	{
		err->messageChar.append("The virtual circuit was reset by the remote side executing a \"hard\" \n");

		err->messageChar.append("or \"abortive\" close. For UPD sockets, the remote host was unable to\n");

		err->messageChar.append("deliver a previously sent UDP datagram and responded with a\n");

		err->messageChar.append("\"Port Unreachable\" ICMP packet. The application should close\n");

		err->messageChar.append("the socket as it is no longer usable.");
	}
	else if (err->errorCode == WSAETIMEDOUT)
	{
		err->messageChar.append("The connection has been dropped, because of a network failure\n");

		err->messageChar.append("or because the system on the other end went down without notice.");
	}
	else err->messageChar.append("unknown problems!");


	return err;
}

void BaseNetService::getErrorOnConnect(NetError& err) const
{
	int errorCode = WSAGetLastError();

	if (errorCode == WSANOTINITIALISED)
		err.messageChar.append("A successful WSAStartup must occur before using this FUNCTION.");
	else if (errorCode == WSAENETDOWN)
		err.messageChar.append("The network subsystem has failed.");
	else if (errorCode == WSAEFAULT)
		err.messageChar.append(
			"The addrlen parameter is too small or addr is not a valid part of the user address space.");
	else if (errorCode == WSAEINTR)
		err.messageChar.append("A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.");
	else if (errorCode == WSAEINPROGRESS)
	{
		err.messageChar.append("A blocking Windows Sockets 1.1 call is in progress, or the\n");
		err.messageChar.append("service provider is still processing a callback function.");
	}
	else if (errorCode == WSAEINVAL)
		err.messageChar.append("The listen function was not invoked prior to accept.");
	else if (errorCode == WSAEMFILE)
		err.messageChar.append("The queue is nonempty upon entry to accept and there are no descriptors available.");
	else if (errorCode == WSAENOBUFS)
		err.messageChar.append("No buffer space is available.");
	else if (errorCode == WSAENOTSOCK)
		err.messageChar.append("The descriptor is not a socket.");
	else if (errorCode == WSAEOPNOTSUPP)
		err.messageChar.append("The referenced socket is not a type that supports connection-oriented service.");
	else if (errorCode == WSAEWOULDBLOCK)
		err.messageChar.append("The socket is marked as nonblocking and no connections are present to be accepted.");
	else err.messageChar.append("unknown problem!");
}

NetError* BaseNetService::getErrorOnReceive()
{
	// AppError* err = (AppError*)GlobalAlloc(0, sizeof AppError);
	NetError* err = new NetError;
	err->errorCode = WSAGetLastError();

	if (err->errorCode == WSANOTINITIALISED)
		err->messageChar.append("A successful WSAStartup must occur before using this function.");
	else if (err->errorCode == WSAENETDOWN)
		err->messageChar.append("The network subsystem has failed.");
	else if (err->errorCode == WSAEADDRINUSE)
	{
		err->messageChar.append("The socket's local address is already in use and the socket\n");
		err->messageChar.append("was not marked to allow address reuse with SO_REUSEADDR. This\n");
		err->messageChar.append("error usually occurs when executing bind, but could be delayed\n");
		err->messageChar.append("until this function if the bind was to a partially wild-card\n");
		err->messageChar.append("address (involving ADDR_ANY) and if a specific address needs\n");
		err->messageChar.append("to be committed at the time of this function.");
	}
	else if (err->errorCode == WSAEINTR)
		err->messageChar.append("The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
	else if (err->errorCode == WSAEINPROGRESS)
	{
		err->messageChar.append("A blocking Windows Sockets 1.1 call is in progress, or\n");
		err->messageChar.append("the service provider is still processing a callback function.");
	}
	else if (err->errorCode == WSAEALREADY)
	{
		err->messageChar.append("A nonblocking connect call is in progress on the specified socket.\n");
		err->messageChar.append("Note In order to preserve backward compatibility, this error is\n");
		err->messageChar.append("reported as WSAEINVAL to Windows Sockets 1.1 applications that\n");
		err->messageChar.append("link to either WINSOCK.DLL or WSOCK32.DLL.");
	}
	else if (err->errorCode == WSAEADDRNOTAVAIL)
		err->messageChar.append("The remote address is not a valid address (such as ADDR_ANY).");
	else if (err->errorCode == WSAEAFNOSUPPORT)
		err->messageChar.append("Addresses in the specified family cannot be used with this socket.");
	else if (err->errorCode == WSAECONNREFUSED)
		err->messageChar.append("The attempt to connect was forcefully rejected.");
	else if (err->errorCode == WSAEFAULT)
	{
		err->messageChar.append("The name or the namelen parameter is not a valid part of\n");
		err->messageChar.append("the user address space, the namelen parameter is too small,\n");
		err->messageChar.append("or the name parameter contains incorrect address format for\n");
		err->messageChar.append("the associated address family.");
	}
	else if (err->errorCode == WSAEINVAL)
	{
		err->messageChar.append("The parameter s is a listening socket, or the destination\n");
		err->messageChar.append("address specified is not consistent with that of the constrained\n");
		err->messageChar.append("group the socket belongs to.");
	}
	else if (err->errorCode == WSAEISCONN)
		err->messageChar.append("The socket is already connected (connection-oriented sockets only).");
	else if (err->errorCode == WSAENETUNREACH)
		err->messageChar.append("The network cannot be reached from this host at this time.");
	else if (err->errorCode == WSAENOBUFS)
		err->messageChar.append("No buffer space is available. The socket cannot be connected.");
	else if (err->errorCode == WSAENOTSOCK)
		err->messageChar.append("The descriptor is not a socket.");
	else if (err->errorCode == WSAETIMEDOUT)
		err->messageChar.append("Attempt to connect timed out without establishing a connection.");
	else if (err->errorCode == WSAEWOULDBLOCK)
	{
		err->messageChar.append("The socket is marked as nonblocking and the connection\n");
		err->messageChar.append("cannot be completed immediately.");
	}
	else if (err->errorCode == WSAEACCES)
	{
		err->messageChar.append("Attempt to connect datagram socket to broadcast address failed\n");
		err->messageChar.append("because setsockopt option SO_BROADCAST is not enabled.");
	}
	else err->messageChar.append("unknown problems!");

	return err;
}

void BaseNetService::connectToHostname(char* hostname, int port)
{
	char* ipAddress = resolveHostName(hostname);
	connectToIpAddress(ipAddress, port);
}

void BaseNetService::connectToIpAddress(const char* ipAddress, int port) const
{
	struct sockaddr_in serverAddress{};
	// if (connect(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);

#ifdef _WINSOCK_DEPRECATED_NO_WARNINGS
	serverAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	// serverAddress.sin_addr.s_addr = inet_addr(ipAddress);
#else
	unsigned long serverAddrNumeric;
	InetPtonA(AF_INET, ipAddress, PVOID(&serverAddrNumeric));
	serverAddress.sin_addr.S_un.S_addr = serverAddrNumeric;
#endif

	try
	{
		if (connect(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
		{
			NetError err;
			getErrorOnConnect(err);
			std::cout << err.messageChar << std::endl;
			return;
		}
	}
	catch (NetError& err)
	{
		// 
		std::cout << err.messageChar << std::endl;
	}
}

char* BaseNetService::resolveHostName(char* hostname)
{
	return nullptr;
	/*
	static int HOST_NAME_LENGTH = 64;
	char szName[HOST_NAME_LENGTH + 1];
	memset(szName, 0, sizeof(szName));
	gethostname(szName, HOST_NAME_LENGTH);

	struct hostent* hostPtr;
	try
	{
		hostPtr = gethostbyname(szName);
		if (hostPtr == NULL)
		{
			// AppError* error = getErrorOnGetHostByName();
			//	throw error;
		}
	}
	catch (AppError& error)
	{
		exit(1);
	}

	struct in_addr* addr_ptr;
	addr_ptr = (struct in_addr*)*hostPtr->h_addr_list;
	char* ipAddress = inet_ntoa(*addr_ptr);
	return ipAddress;
	*/
}

/*
int BaseNetService::sendMessage(char* message)
{
	int numBytes; // the number of bytes sent

	/*
	   for each message to be sent, add a header which shows how long this message
	   is. This header, regardless how long the real message is, will always be
	   of the length MSG_HEADER_LEN.
	*/
/*
	char msgLength[PACKET_HEADER_LEN + 1];
	// sprintf(msgLength, "%6d", strlen(message));
	std::string sendMsg = std::string(msgLength);
	sendMsg += message;

	// Sends the message to the connected host
	try
	{
		if (numBytes = send(socketDescriptor, sendMsg.c_str(), sendMsg.size(), 0) == -1)
		{
			AppError* err = getErrorOnSend();
			throw err;
		}
	}
	catch (AppError& excp)
	{
		exit(1);
	}

	return numBytes;
}



int BaseNetService::receivePacket(char* messageBuffer)
{
	int received = 0; // The number of bytes received
	int msgSize = MAX_RECV_LEN; // The number of bytes wanted to receive
	int numBytes = 0; // The number of bytes currently recieved
	int totalRecvNum = 0;
	bool headerFinished = false;

	char charMsg[MAX_RECV_LEN + 1];
	char msgLength[PACKET_HEADER_LEN + 1];
	memset(charMsg, 0, sizeof(charMsg));
	memset(msgLength, 0, sizeof(msgLength));

	try
	{
		while (received < msgSize)
		{
			numBytes = recv(socketDescriptor, charMsg + received, 1, 0);
			if (numBytes == -1)
			{
				int errorCode = 0;
				AppError* err = getErrorOnReceive();
				throw err;
			}

			if (!headerFinished)
			{
				msgLength[received] = *(charMsg + received);
				received++;

				if (received == PACKET_HEADER_LEN)
				{
					headerFinished = true;
					received = 0;
					memset(charMsg, 0, sizeof(charMsg));
					msgSize = atoi(msgLength);
				}
			}
			else
				received++;
		}
	}
	catch (AppError& excp)
	{
		if (excp.errorCode == WSAECONNRESET)
		{
			//cout   << "!! your party has shut down the connection... \n";
			//winLog << "!! your party has shut down the connection... \n";
			return -99;
		}

		exit(1);
	}

	// message.append(std::string(charMsg));
	return msgSize;
}

*/

NetError* BaseNetService::getErrorOnSocketCreate()
{
	auto err = new NetError;
	int errorCode = WSAGetLastError();
	err->errorCode = errorCode;
	if (errorCode == WSANOTINITIALISED)
		err->messageChar.append("Successful WSAStartup must occur before using this function.");
	else if (errorCode == WSAENETDOWN)
		err->messageChar.append("The network subsystem or the associated service provider has failed.");
	else if (errorCode == WSAEAFNOSUPPORT)
		err->messageChar.append("The specified address family is not supported.");
	else if (errorCode == WSAEINPROGRESS)
		err->messageChar.append(
			"A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
	else if (errorCode == WSAEMFILE)
		err->messageChar.append("No more socket descriptors are available.");
	else if (errorCode == WSAENOBUFS)
		err->messageChar.append("No buffer space is available. The socket cannot be created.");
	else if (errorCode == WSAEPROTONOSUPPORT)
		err->messageChar.append("The specified protocol is not supported.");
	else if (errorCode == WSAEPROTOTYPE)
		err->messageChar.append("The specified protocol is the wrong type for this socket.");
	else if (errorCode == WSAESOCKTNOSUPPORT)
		err->messageChar.append("The specified socket type is not supported in this address family.");
	else err->messageChar.append("unknown problems!");

	return err;
}

void BaseNetService::setSocketDescriptor(SOCKET socket)
{
	socketDescriptor = socket;
}
