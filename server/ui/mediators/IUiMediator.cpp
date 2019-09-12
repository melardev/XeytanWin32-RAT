#include "IUiMediator.h"


IUiMediator::IUiMediator()
{
}


IUiMediator::~IUiMediator()
{
}

void IUiMediator::showErrorMessage(const std::string& errorMessage)
{
	showErrorMessage(errorMessage.c_str());
}
