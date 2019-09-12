#pragma once
#include <Rpc.h>

class UuidService
{
public:
	UuidService();
	~UuidService();
	static UUID* generateUUID();
	static void freeUuid(const UUID* uuid);
	static bool getStringFromUuid(UUID* uuid, const char buffer[]);
};
