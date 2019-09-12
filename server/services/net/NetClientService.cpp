#include "NetClientService.h"
#include "NetServerService.h"
#include "../../../shared/NetLib/packets/PacketSysInfo.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../../shared/ConcurrentLib/core/Thread.h"
#include "../../models/Client.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketDownload.h"
#include "../../../shared/NetLib/packets/PacketMediaResponse.h"
#include "../../../shared/NetLib/packets/PacketShell.h"


NetClientService::NetClientService(UUID* uuid, NetServerService* server, SOCKET clientSocket)
	: BaseNetClientService(clientSocket),
	  netServerService(server),
	  client(new Client(uuid, clientSocket))
{
	receivedPacket = new Packet;
}

NetClientService::~NetClientService()
{
}


Client* NetClientService::getClient()
{
	return client;
}


void NetClientService::onPacketReceived(PacketType packetType, char* payloadBuffer, size_t payloadLength)
{
	Packet* packet;
	if (packetType == PacketType::Presentation)
	{
		PacketPresentation* packetPresentation = PacketPresentation::deserialize(payloadBuffer,
		                                                                         payloadLength);
		client->setUsername(packetPresentation->getUsername());
		client->setPcName(packetPresentation->getPcname());
		packet = packetPresentation;
	}
	else if (packetType == PacketType::ClientInformation)
		packet = PacketSysInfo::deserialize(payloadBuffer,
		                                    payloadLength);

	else if (packetType == PacketType::ProcessList)
		packet = PacketProcess::deserialize(payloadBuffer,
		                                    payloadLength);
	else if (packetType == PacketType::FileSystem)
		packet = PacketFileSystem::deserialize(payloadBuffer,
		                                       payloadLength);
	else if (packetType == PacketType::Download)
		packet = PacketDownload::deserialize(payloadBuffer,
		                                     payloadLength);
	else if (packetType == PacketType::Desktop)
		packet = PacketMediaResponse::deserialize(packetType, payloadBuffer,
		                                          payloadLength);
	else if (packetType == PacketType::ReverseShell)
		packet = PacketShell::deserialize(payloadBuffer, payloadLength);
	else
		return;

	netServerService->onPacketReceived(client, packet);
}

void NetClientService::onDisconnected()
{
	netServerService->onClientDisconnected(client);
}
