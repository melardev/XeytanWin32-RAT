#include "MemoryManagementService.h"


MemoryManagementService::MemoryManagementService()
{
}


MemoryManagementService::~MemoryManagementService()
{
}

void* MemoryManagementService::allocate(unsigned long long size)
{
	return nullptr;
}

bool MemoryManagementService::realloc(LPSTR* buffer, unsigned long long size)
{
	return true;
}
