#pragma once
#include <Windows.h>

class MemoryManagementService
{
public:
	MemoryManagementService();
	~MemoryManagementService();
	void* allocate(unsigned long long size);
	bool realloc(LPSTR* buffer, unsigned long long size);
};

