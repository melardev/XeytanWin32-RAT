#pragma once
#include "BaseNetService.h"
#include "../packets/packetdefs.h"

class Packet;
class Client;
class Thread;

class BaseNetClientService : public BaseNetService
{
public:
	BaseNetClientService(SOCKET clientSocket);
	BaseNetClientService();
	~BaseNetClientService();

	void interactAsync();
	void interact();

	bool readHeader();
	bool readBody() const;

	virtual void onDisconnected() = 0;
	virtual void onPacketReceived(PacketType packet, char* payloadBuffer, size_t payloadLength) = 0;

	virtual void sendPacket(Packet* packet);

	/*
	UUID* getUuid();
	*/

protected:
	Thread* thread;
	bool shouldBeRunning;
	Packet* receivedPacket;
};
