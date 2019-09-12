#pragma once

#include <rpc.h>

class UuidService
{
public:
	UuidService();
	~UuidService();
	UUID* generateUUID();
	void freeUuid(const UUID* uuid);
	bool getStringFromUuid(UUID* uuid, const char buffer[]);
};
