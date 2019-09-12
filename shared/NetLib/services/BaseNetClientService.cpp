#include "BaseNetClientService.h"
#include "../../../shared/NetLib/packets/PacketSysInfo.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../../shared/ConcurrentLib/core/Thread.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"

DWORD WINAPI clientThreadFunc(LPVOID threadArg)
{
	BaseNetClientService* arg = (BaseNetClientService*)(threadArg);
	arg->interact();
	return 0;
}

BaseNetClientService::BaseNetClientService(SOCKET clientSocket)
	: BaseNetService(clientSocket)
{
	receivedPacket = new Packet;
}

BaseNetClientService::BaseNetClientService() : BaseNetService()
{
	receivedPacket = new Packet;
}

BaseNetClientService::~BaseNetClientService()
{
}


void BaseNetClientService::interactAsync()
{
	auto thread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(clientThreadFunc),
	                         static_cast<void*>(this));
	this->thread = thread;
	thread->run();
}

void BaseNetClientService::interact()
{
	if (socketDescriptor == INVALID_SOCKET)
	{
		// Forgot to call setSocketDescriptor
		int a = 0;
	}
	PacketType PacketType;
	Packet* packet;
	shouldBeRunning = true;
	while (shouldBeRunning)
	{
		receivedPacket->invalidate();
		if (!readHeader())
		{
			onDisconnected();
			return;
		}
		if (receivedPacket->hasValidHeader())
		{
			if (readBody())
			{
				const ::PacketType packetType = receivedPacket->getType();
				char* payloadBuffer = receivedPacket->getPayloadBuffer();
				const size_t payloadLength = receivedPacket->getPayloadLength();
				onPacketReceived(packetType, payloadBuffer, payloadLength);
			}
		}
		else
		{
			return;
		}
	}
}

bool BaseNetClientService::readHeader()
{
	int numBytes = 0;

	char* buffer = receivedPacket->getHeaderBuffer();
	while (numBytes < PACKET_HEADER_LENGTH)
	{
		numBytes += recv(socketDescriptor, buffer + numBytes, PACKET_HEADER_LENGTH - numBytes, 0);
		if (numBytes == SOCKET_ERROR || numBytes == 0)
			return false;
	}
	return true;
}

bool BaseNetClientService::readBody() const
{
	char* buffer = receivedPacket->getPayloadBuffer();
	const int32_t payloadLength = receivedPacket->getPayloadLength();

	int numBytes = 0;
	while (numBytes < payloadLength)
		numBytes += recv(socketDescriptor, buffer + numBytes, payloadLength - numBytes, 0);

	return true;
}

void BaseNetClientService::sendPacket(Packet* packet)
{
	char* serialized = packet->getSerialized();
	const size_t packetSize = packet->getTotalPacketSize();

	int sendBytes = 0;
	while (sendBytes < packetSize)
	{
		sendBytes += send(socketDescriptor, serialized + sendBytes, packetSize - sendBytes, 0);
		if (sendBytes == 0 || sendBytes == SOCKET_ERROR)
			return;
	}

	delete packet;
}
