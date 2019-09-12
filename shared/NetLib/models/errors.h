#pragma once
#include <string>

typedef struct NetError
{
	int errorCode;
	std::string messageChar;
} *PNET_ERROR, *LP_NET_ERROR;
