#include "NetService.h"
#include <iostream>


void NetService::initWinsock2()
{
	static WSADATA wsaData;


	if (strcmp(wsaData.szSystemStatus, "Running") != 0)
	{
		DWORD result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			AppError initializationException{0, "Error: calling WSAStartup()"};
			throw initializationException;
		}
	}
}

NetService::NetService()
{
	initWinsock2();
}

NetService::NetService(SOCKET sock) : socketDescriptor(sock)
{
	initWinsock2();
}

NetService::~NetService()
{
}

void NetService::connectToHostname(char* hostname, int port)
{
	char* ipAddress = resolveHostName(hostname);
	connectToIpAddress(ipAddress, port);
}

void NetService::connectToIpAddress(const char* ipAddress, int port)
{
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(ipAddress);
	serverAddress.sin_port = htons(port);
	
	if (connect(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		AppError* error = getErrorOnConnect();
		throw error;
	}
}

char* NetService::resolveHostName(char* hostname)
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

SOCKET NetService::getSocket()
{
	return socketDescriptor;
}

void NetService::setSocket(SOCKET conSocket)
{
	socketDescriptor = conSocket;
}

AppError* NetService::getErrorOnSend()
{
	// TODO: Messages Copied, change messages
	AppError* err = (AppError*)GlobalAlloc(0, sizeof AppError);
	err->errorCode = WSAGetLastError();

	if (err->errorCode == WSANOTINITIALISED)
		err->messageChar = "A successful WSAStartup must occur before using this function.";

	else if (err->errorCode == WSAENETDOWN)
		err->messageChar = "The network subsystem has failed.";

	else if (err->errorCode == WSAEACCES)
	{
		err->messageChar = "The requested address is a broadcast address,\n";

		err->messageChar = "but the appropriate flag was not set. Call setsockopt\n";

		err->messageChar = "with the SO_BROADCAST parameter to allow the use of the broadcast address.";
	}
	else if (err->errorCode == WSAEINTR)
	{
		err->messageChar = "A blocking Windows Sockets 1.1 call was canceled\n";

		err->messageChar = "through WSACancelBlockingCall.";
	}
	else if (err->errorCode == WSAEINPROGRESS)
	{
		err->messageChar = "A blocking Windows Sockets 1.1 call is in progress,\n";

		err->messageChar = "or the service provider is still processing a callback function.";
	}
	else if (err->errorCode == WSAEFAULT)
	{
		err->messageChar = "The buf parameter is not completely contained in a\n";

		err->messageChar = "valid part of the user address space.";
	}
	else if (err->errorCode == WSAENETRESET)
	{
		err->messageChar = "The connection has been broken due to the keep-alive\n";

		err->messageChar = "activity detecting a failure while the operation was in progress.";
	}
	else if (err->errorCode == WSAENOBUFS)
		err->messageChar = "No buffer space is available.";

	else if (err->errorCode == WSAENOTCONN)
		err->messageChar = "The socket is not connected.";

	else if (err->errorCode == WSAENOTSOCK)
		err->messageChar = "The descriptor is not a socket.";

	else if (err->errorCode == WSAEOPNOTSUPP)
	{
		err->messageChar = "MSG_OOB was specified, but the socket is not stream-style\n";

		err->messageChar = "such as type SOCK_STREAM, out-of-band data is not supported\n";

		err->messageChar = "in the communication domain associated with this socket,\n";

		err->messageChar = "or the socket is unidirectional and supports only receive operations.";
	}
	else if (err->errorCode == WSAESHUTDOWN)
	{
		err->messageChar = "The socket has been shut down; it is not possible to send\n";

		err->messageChar = "on a socket after shutdown has been invoked with how set\n";

		err->messageChar = "to SD_SEND or SD_BOTH.";
	}
	else if (err->errorCode == WSAEWOULDBLOCK)
		err->messageChar = "The socket is marked as nonblocking and the requested operation would block.\n";

	else if (err->errorCode == WSAEMSGSIZE)
	{
		err->messageChar = "The socket is message oriented, and the message is larger\n";

		err->messageChar = "than the maximum supported by the underlying transport.";
	}
	else if (err->errorCode == WSAEHOSTUNREACH)
		err->messageChar = "The remote host cannot be reached from this host at this time.";

	else if (err->errorCode == WSAEINVAL)
	{
		err->messageChar = "The socket has not been bound with bind, or an unknown flag\n";

		err->messageChar = "was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.";
	}
	else if (err->errorCode == WSAECONNABORTED)
	{
		err->messageChar = "The virtual circuit was terminated due to a time-out or \n";

		err->messageChar = "other failure. The application should close the socket as it is no longer usable.";
	}
	else if (err->errorCode == WSAECONNRESET)
	{
		err->messageChar = "The virtual circuit was reset by the remote side executing a \"hard\" \n";

		err->messageChar = "or \"abortive\" close. For UPD sockets, the remote host was unable to\n";

		err->messageChar = "deliver a previously sent UDP datagram and responded with a\n";

		err->messageChar = "\"Port Unreachable\" ICMP packet. The application should close\n";

		err->messageChar = "the socket as it is no longer usable.";
	}
	else if (err->errorCode == WSAETIMEDOUT)
	{
		err->messageChar = "The connection has been dropped, because of a network failure\n";

		err->messageChar = "or because the system on the other end went down without notice.";
	}
	else err->messageChar = "unknown problems!";


	return err;
}

AppError* NetService::getErrorOnReceive()
{
	AppError* err = (AppError*)GlobalAlloc(0, sizeof AppError);
	err->errorCode = WSAGetLastError();

	if (err->errorCode == WSANOTINITIALISED)
		err->messageChar = "A successful WSAStartup must occur before using this function.";
	else if (err->errorCode == WSAENETDOWN)
		err->messageChar = "The network subsystem has failed.";
	else if (err->errorCode == WSAEADDRINUSE)
	{
		err->messageChar = "The socket's local address is already in use and the socket\n";
		err->messageChar = "was not marked to allow address reuse with SO_REUSEADDR. This\n";
		err->messageChar = "error usually occurs when executing bind, but could be delayed\n";
		err->messageChar = "until this function if the bind was to a partially wild-card\n";
		err->messageChar = "address (involving ADDR_ANY) and if a specific address needs\n";
		err->messageChar = "to be committed at the time of this function.";
	}
	else if (err->errorCode == WSAEINTR)
		err->messageChar = "The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
	else if (err->errorCode == WSAEINPROGRESS)
	{
		err->messageChar = "A blocking Windows Sockets 1.1 call is in progress, or\n";
		err->messageChar = "the service provider is still processing a callback function.";
	}
	else if (err->errorCode == WSAEALREADY)
	{
		err->messageChar = "A nonblocking connect call is in progress on the specified socket.\n";
		err->messageChar = "Note In order to preserve backward compatibility, this error is\n";
		err->messageChar = "reported as WSAEINVAL to Windows Sockets 1.1 applications that\n";
		err->messageChar = "link to either WINSOCK.DLL or WSOCK32.DLL.";
	}
	else if (err->errorCode == WSAEADDRNOTAVAIL)
		err->messageChar = "The remote address is not a valid address (such as ADDR_ANY).";
	else if (err->errorCode == WSAEAFNOSUPPORT)
		err->messageChar = "Addresses in the specified family cannot be used with this socket.";
	else if (err->errorCode == WSAECONNREFUSED)
		err->messageChar = "The attempt to connect was forcefully rejected.";
	else if (err->errorCode == WSAEFAULT)
	{
		err->messageChar = "The name or the namelen parameter is not a valid part of\n";
		err->messageChar = "the user address space, the namelen parameter is too small,\n";
		err->messageChar = "or the name parameter contains incorrect address format for\n";
		err->messageChar = "the associated address family.";
	}
	else if (err->errorCode == WSAEINVAL)
	{
		err->messageChar = "The parameter s is a listening socket, or the destination\n";
		err->messageChar = "address specified is not consistent with that of the constrained\n";
		err->messageChar = "group the socket belongs to.";
	}
	else if (err->errorCode == WSAEISCONN)
		err->messageChar = "The socket is already connected (connection-oriented sockets only).";
	else if (err->errorCode == WSAENETUNREACH)
		err->messageChar = "The network cannot be reached from this host at this time.";
	else if (err->errorCode == WSAENOBUFS)
		err->messageChar = "No buffer space is available. The socket cannot be connected.";
	else if (err->errorCode == WSAENOTSOCK)
		err->messageChar = "The descriptor is not a socket.";
	else if (err->errorCode == WSAETIMEDOUT)
		err->messageChar = "Attempt to connect timed out without establishing a connection.";
	else if (err->errorCode == WSAEWOULDBLOCK)
	{
		err->messageChar = "The socket is marked as nonblocking and the connection\n";
		err->messageChar = "cannot be completed immediately.";
	}
	else if (err->errorCode == WSAEACCES)
	{
		err->messageChar = "Attempt to connect datagram socket to broadcast address failed\n";
		err->messageChar = "because setsockopt option SO_BROADCAST is not enabled.";
	}
	else err->messageChar = "unknown problems!";

	return err;
}


/*
int NetService::sendMessage(char* message)
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
		if (numBytes = send(connectionSocket, sendMsg.c_str(), sendMsg.size(), 0) == -1)
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


/*
int NetService::receivePacket(char* messageBuffer)
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
			numBytes = recv(connectionSocket, charMsg + received, 1, 0);
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
