#include "UuidService.h"


UuidService::UuidService()
{
}


UuidService::~UuidService()
{
}

UUID* UuidService::generateUUID()
{
	UUID* uuid = new UUID;
	if (UuidCreate(uuid) == RPC_S_OK)
	{
		return uuid;
	}
	return nullptr;
}

bool UuidService::getStringFromUuid(UUID* uuid, const char buffer[])
{
	if (UuidToStringA(uuid, (RPC_CSTR*)buffer) == RPC_S_OK)
		return true;
	return false;
}

void UuidService::freeUuid(const UUID* uuid)
{
	RpcStringFreeA((RPC_CSTR*)uuid);
}
