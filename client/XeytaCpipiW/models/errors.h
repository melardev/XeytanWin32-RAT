#pragma once
#include <string>

typedef struct AppError
{
	int errorCode;
	std::string messageChar;
} *PAPP_ERROR, *LP_APP_ERROR;
