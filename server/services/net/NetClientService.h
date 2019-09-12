#pragma once
#include "../../../shared/NetLib/services/BaseNetClientService.h"

class Packet;
class Client;
class NetServerService;
class Thread;

class NetClientService : public BaseNetClientService
{
public:
	NetClientService(UUID* uuid, NetServerService* server, SOCKET clientSocket);
	~NetClientService();

	virtual void onDisconnected() override;
	virtual void onPacketReceived(PacketType packet, char* payloadBuffer, size_t payloadLength) override;

	Client* getClient();

private:

	NetServerService* netServerService;
	Client* client;
};
