#pragma once

#include <WinSock2.h>
#include "../models/errors.h"

class NetService
{
public:
	void initWinsock2();
	NetService();
	NetService(SOCKET sock);
	~NetService();

	AppError* getErrorOnSend();

	AppError* getErrorOnConnect()
	{
		int errCode = WSAGetLastError();
		AppError* appError = new AppError;
		appError->errorCode = errCode;

		if (errCode == WSANOTINITIALISED)
			appError->messageChar.append("A successful WSAStartup must occur before using this function.");
		else if (errCode == WSAENETDOWN)
			appError->messageChar.append("The network subsystem has failed.");
		else if (errCode == WSAEADDRINUSE)
		{
			appError->messageChar.append("The socket's local address is already in use and the socket\n");
			appError->messageChar.append("was not marked to allow address reuse with SO_REUSEADDR. This\n");
			appError->messageChar.append("error usually occurs when executing bind, but could be delayed\n");
			appError->messageChar.append("until this function if the bind was to a partially wild-card\n");
			appError->messageChar.append("address (involving ADDR_ANY) and if a specific address needs\n");
			appError->messageChar.append("to be committed at the time of this function.");
		}
		else if (errCode == WSAEINTR)
			appError->messageChar.append("The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
		else if (errCode == WSAEINPROGRESS)
		{
			appError->messageChar.append("A blocking Windows Sockets 1.1 call is in progress, or\n");
			appError->messageChar.append("the service provider is still processing a callback function.");
		}
		else if (errCode == WSAEALREADY)
		{
			appError->messageChar.append("A nonblocking connect call is in progress on the specified socket.\n");
			appError->messageChar.append("Note In order to preserve backward compatibility, this error is\n");
			appError->messageChar.append("reported as WSAEINVAL to Windows Sockets 1.1 applications that\n");
			appError->messageChar.append("link to either WINSOCK.DLL or WSOCK32.DLL.");
		}
		else if (errCode == WSAEADDRNOTAVAIL)
			appError->messageChar.append("The remote address is not a valid address (such as ADDR_ANY).");
		else if (errCode == WSAEAFNOSUPPORT)
			appError->messageChar.append("Addresses in the specified family cannot be used with this socket.");
		else if (errCode == WSAECONNREFUSED)
			appError->messageChar.append("The attempt to connect was forcefully rejected.");
		else if (errCode == WSAEFAULT)
		{
			appError->messageChar.append("The name or the namelen parameter is not a valid part of\n");
			appError->messageChar.append("the user address space, the namelen parameter is too small,\n");
			appError->messageChar.append("or the name parameter contains incorrect address format for\n");
			appError->messageChar.append("the associated address family.");
		}
		else if (errCode == WSAEINVAL)
		{
			appError->messageChar.append("The parameter s is a listening socket, or the destination\n");
			appError->messageChar.append("address specified is not consistent with that of the constrained\n");
			appError->messageChar.append("group the socket belongs to.");
		}
		else if (errCode == WSAEISCONN)
			appError->messageChar.append("The socket is already connected (connection-oriented sockets only).");
		else if (errCode == WSAENETUNREACH)
			appError->messageChar.append("The network cannot be reached from this host at this time.");
		else if (errCode == WSAENOBUFS)
			appError->messageChar.append("No buffer space is available. The socket cannot be connected.");
		else if (errCode == WSAENOTSOCK)
			appError->messageChar.append("The descriptor is not a socket.");
		else if (errCode == WSAETIMEDOUT)
			appError->messageChar.append("Attempt to connect timed out without establishing a connection.");
		else if (errCode == WSAEWOULDBLOCK)
		{
			appError->messageChar.append("The socket is marked as nonblocking and the connection\n");
			appError->messageChar.append("cannot be completed immediately.");
		}
		else if (errCode == WSAEACCES)
		{
			appError->messageChar.append("Attempt to connect datagram socket to broadcast address failed\n");
			appError->messageChar.append("because setsockopt option SO_BROADCAST is not enabled.");
		}
		else appError->messageChar.append("unknown problems!");

		return appError;
	}

	AppError* getErrorOnReceive();

	void connectToHostname(char* hostname, int port);

	void connectToIpAddress(const char* ipAddress, int port);

	char* resolveHostName(char* hostname);

	SOCKET getSocket();
	void setSocket(SOCKET conSocket);
	int sendMessage(char* message);

	int receivePacket(char* messageBuffer);

protected:
	SOCKET socketDescriptor;
};
