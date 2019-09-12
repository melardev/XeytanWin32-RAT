#pragma once
#include <string>

enum UiMode
{
	Win32Console,
	Win32Graphical
};

class IUiMediator
{
public:
	IUiMediator();
	virtual ~IUiMediator();
	virtual void showMainUi() = 0;
	virtual void loop() = 0;
	virtual void updateStatus(const std::string& statusMessage) = 0;
	virtual void showErrorMessage(const char* str) =0;
	void showErrorMessage(const std::string& errorMessage);
	virtual void addFeedbackMessage(const std::string& message) =0;
};
