#pragma once
#include "stdafx.h"
#include <Wincrypt.h>

// DEFINITIONS
typedef NTSTATUS(__stdcall *fNtCreateFile)
(OUT PHANDLE FileHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PLARGE_INTEGER AllocationSize OPTIONAL,
	IN ULONG FileAttributes,
	IN ULONG ShareAccess,
	IN ULONG CreateDisposition,
	IN ULONG CreateOptions,
	IN PVOID EaBuffer OPTIONAL,
	IN ULONG EaLength
);

typedef BOOL(__stdcall *fCryptCATAdminCalcHashFromFileHandle)
(
	_In_    HANDLE hFile,
	_Inout_ DWORD  *pcbHash,
	_In_    BYTE   *pbHash,
	_In_    DWORD  dwFlags
);

typedef NTSTATUS(NTAPI *fNtReadVirtualMemory)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	OUT PVOID Buffer,
	IN ULONG BufferLength,
	OUT PULONG ReturnLength OPTIONAL
);

typedef enum _MEMORY_INFORMATION_CLASS {
	MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
	MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
	MemoryMappedFilenameInformation, // UNICODE_STRING
	MemoryRegionInformation, // MEMORY_REGION_INFORMATION
	MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
	MemorySharedCommitInformation // MEMORY_SHARED_COMMIT_INFORMATION
} MEMORY_INFORMATION_CLASS;



typedef NTSTATUS(NTAPI *fNtQueryVirtualMemory)
(
	IN HANDLE               ProcessHandle,
	IN PVOID                BaseAddress,
	IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
	OUT PVOID               Buffer,
	IN ULONG                Length,
	OUT PULONG              ResultLength OPTIONAL
);

typedef struct _DEBUG_BUFFER {
	HANDLE  SectionHandle;
	PVOID  SectionBase;
	PVOID  RemoteSectionBase;
	ULONG  SectionBaseDelta;
	HANDLE  EventPairHandle;
	ULONG  Unknown[2];
	HANDLE  RemoteThreadHandle;
	ULONG  InfoClassMask;
	ULONG  SizeOfInfo;
	ULONG  AllocatedSize;
	ULONG  SectionSize;
	PVOID  ModuleInformation;
	PVOID  BackTraceInformation;
	PVOID  HeapInformation;
	PVOID  LockInformation;
	PVOID  Reserved[8];
} DEBUG_BUFFER, *PDEBUG_BUFFER;

typedef NTSTATUS(NTAPI *fNtQuerySystemInformation)(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	);

typedef VOID(__stdcall *_RtlInitUnicodeString)
(
	PUNICODE_STRING DestinationString,
	PCWSTR SourceString
	);


typedef DWORD(WINAPI *fGetDeviceDriverFileName)(
	_In_  LPVOID ImageBase,
	_Out_ LPTSTR lpFilename,
	_In_  DWORD  nSize
);

typedef BOOL (WINAPI *fK32EnumDeviceDrivers)(
	_Out_ LPVOID  *lpImageBase,
	_In_  DWORD   cb,
	_Out_ LPDWORD lpcbNeeded
);

typedef PCCERT_CONTEXT (WINAPI *fCertEnumCertificatesInStore)(
	_In_ HCERTSTORE     hCertStore,
	_In_ PCCERT_CONTEXT pPrevCertContext
);

typedef BOOL (WINAPI *fEnumWindows)
(
	_In_ WNDENUMPROC lpEnumFunc,
	_In_ LPARAM      lParam
);


namespace Fuckers //Hooks
{
	//ORIGINAL FUNCTIONS

	//fuck be's file check
	extern fNtCreateFile NtCreateFileOrig;
	extern fCryptCATAdminCalcHashFromFileHandle CryptCATAdminCalcHashFromFileHandleOrig;

	extern fNtQueryVirtualMemory NtQueryVirtualMemoryOrig;
	extern fNtReadVirtualMemory NtReadVirtualMemoryOrig;
	extern fNtQuerySystemInformation NtQuerySystemInformationOrig;

	extern fK32EnumDeviceDrivers K32EnumDeviceDriversOrig;
	extern fCertEnumCertificatesInStore CertEnumCertificatesInStoreOrig;

	extern fEnumWindows EnumWindowsOrig;

	//HOOK FUNCTIONS
	NTSTATUS NTAPI NtCreateFileHook(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, void* IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
	BOOL WINAPI CryptCATAdminCalcHashFromFileHandleHook(HANDLE hFile, DWORD * pcbHash, BYTE * pbHash, DWORD dwFlags);

	NTSTATUS NTAPI NtReadVirtualMemoryHook(HANDLE ProcessHandle,PVOID BaseAddress,PVOID Buffer,ULONG BufferLength,PULONG ReturnLength OPTIONAL);
	NTSTATUS NTAPI NtQueryVirtualMemoryHook(HANDLE ProcessHandle,PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass,PVOID Buffer,ULONG Length,PULONG ResultLength OPTIONAL);
	NTSTATUS NTAPI NtQuerySystemInformationHook(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

	BOOL WINAPI K32EnumDeviceDriversHook(
		 LPVOID  *lpImageBase,
		  DWORD   cb,
		 LPDWORD lpcbNeeded
	);

	PCCERT_CONTEXT WINAPI CertEnumCertificatesInStoreHook(
		HCERTSTORE     hCertStore,
		PCCERT_CONTEXT pPrevCertContext
	);


	BOOL WINAPI EnumWindowsHook(
		WNDENUMPROC lpEnumFunc,
		LPARAM      lParam
	);


}