#pragma once
#include "../../shared/NetLib/packets/PacketShell.h"
#include "ShellService.h"

class Thread;
class FileSystemService;
class SystemInfoService;
class ProcessService;
class NetClientService;
class PacketFileSystem;
class PacketDownload;
class PacketPresentation;

class Application
{
public:

	Application();
	static Application* start();

	void startNetSubsystem();

	bool run();
	void onProcessListRequested() const;
	void onFileSystemRequested(PacketFileSystem* packetFileSystem) const;
	void downloadRequested(PacketDownload* packetDownload);
	void desktopRequested() const;


	~Application();

	void onClientExited(const char* str);
	void onConnectionEstablished();
	void onSystemInformationRequested(PacketPresentation* packet);
	void onShellClosed() const;
	void onShellOutput(char* buffer, unsigned long bufferLen) const;
	void shellAction(PacketShellAction action, const std::string& data);


private:
	NetClientService* netClientService{};
	Thread* netClientThread{};
	ShellService shellService;
};
