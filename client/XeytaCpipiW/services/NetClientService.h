#pragma once
#include "../../../shared/NetLib/services/BaseNetClientService.h"
#include "../../../../Win32/ConcurrentLib/pool/BasicThreadPool.h"


class Application;
class Packet;
class Thread;

enum class PacketType;

class NetClientService : public BaseNetClientService
{
public:

	NetClientService(Application* application);
	~NetClientService();
	bool ReceivePacket(PacketType packet);

	void handlePacket(const Packet* packet);
	bool HandleSendFile(int Id);

	UUID* getUuid();
	DWORD start();

	virtual void onDisconnected() override;
	virtual void onPacketReceived(PacketType packet, char* payloadBuffer, size_t payloadLength) override;
	void sendPacket(Packet* packet) override;
	void loopPacketsQueue();
private:

	UUID* clientUuid;
	sockaddr_in addr;
	Application* application;
	BasicThreadPool threadPool;

	std::queue<Packet*> packetsToBeSent;
	std::condition_variable cv;
	std::mutex outPacketsMutex;
	Thread* packetSendThread;
};
