#pragma once
#include "../../../appdefs.h"
#include "../../mediators/IUiMediator.h"
#include "../AppConsole.h"
#include "../../../../shared/ConcurrentLib/pool/BasicThreadPool.h"
#include <Windows.h>

class Application;


class SimpleConsoleUiMediator : public IUiMediator
{
public:
	SimpleConsoleUiMediator(Application* uiChannel);

	~SimpleConsoleUiMediator();
	void showMainUi() override;

	void printHelp();
	bool handleInstruction(char* instruction);
	void loop() override;
	std::wstring getFilePath(char* instruction);

	void stop();

	virtual void updateStatus(const std::string& statusMessage) override;
	virtual void showErrorMessage(const char* str) override;
	void showErrorMessage(const std::string& str);
	virtual void addFeedbackMessage(const std::string& message) override;
	void showImage(char* image_bytes, unsigned long long image_size);
	void addClientRow(Client* client);
	void showProcessList(Client* client, std::vector<Process>* processes);
	void removeClient(Client* client);
	void onShellOutput(Client* client, const std::string& data);
	void onShellClosed(Client* client);

// private:
	bool running;
	AppConsole appConsole;

	bool isStreamingDesktop;
	Application* application;

	BasicThreadPool threadPool;

	SRWLOCK viewLock;
	SRWLOCK clientLock;
};
