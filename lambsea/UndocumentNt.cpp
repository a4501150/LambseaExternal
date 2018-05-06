#pragma once
#include "UndocumentNt.h"
#include <ntstatus.h>
fNtQuerySystemInformation NtQuerySystemInformation = (fNtQuerySystemInformation)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQuerySystemInformation");

struct UNICODE_STRING
{
	WORD		Length;
	WORD		MaxLength;
	wchar_t *	szBuffer;
};

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	ULONG                   BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
}SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;


HANDLE NtQueryProcessStatus(HANDLE watchedPID)
{
	NTSTATUS status;
	PVOID buffer;
	PSYSTEM_PROCESS_INFO psi = { 0 };
	ULONG ReturnLenght = 0;


	status = NtQuerySystemInformation(SystemProcessInformation, NULL, NULL, &ReturnLenght);

	if (status != STATUS_INFO_LENGTH_MISMATCH && !NT_SUCCESS(status))
		return (HANDLE)STATUS_ACCESS_DENIED;

	buffer = VirtualAlloc(NULL, ReturnLenght, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!NT_SUCCESS(status = NtQuerySystemInformation(SystemProcessInformation, buffer, ReturnLenght, NULL)))
	{
		VirtualFree(buffer, 0, MEM_RELEASE);
		//printf("failed status is 0x%I64X", status);
		return (HANDLE)status;
	}

	psi = (PSYSTEM_PROCESS_INFO)buffer;

	while (psi->NextEntryOffset) // Loop over the list until we reach the last entry.
	{
		if (psi->ProcessId == watchedPID)
		{
			//printf("founded!\n");
			HANDLE tmp = psi->ProcessId;
			VirtualFree(psi, 0, MEM_RELEASE);
			return tmp;
		}
		psi = (PSYSTEM_PROCESS_INFO)((LPBYTE)psi + psi->NextEntryOffset); // Calculate the address of the next entry.
	}

	VirtualFree(psi, 0, MEM_RELEASE);
	return (HANDLE)0xdeadbeef;
}
