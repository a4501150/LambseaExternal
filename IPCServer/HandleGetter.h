#pragma once
#include "stdafx.h"
#include <functional>
#include <ntstatus.h>
#include "winternl.h"


typedef struct _OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION
{
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccessMask;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;
	ULONG PoolType;
	ULONG DefaultPagedPoolCharge;
	ULONG DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_HANDLE_ATTRIBUTE_INFORMATION
{
	BOOLEAN Inherit;
	BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_ATTRIBUTE_INFORMATION, *POBJECT_HANDLE_ATTRIBUTE_INFORMATION;

typedef struct _OBJECT_DIRECTORY_INFORMATION
{
	UNICODE_STRING Name;
	UNICODE_STRING TypeName;
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

typedef struct _OBJECT_HANDLE_FLAG_INFORMATION {
	BOOLEAN Inherit;
	BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_FLAG_INFORMATION, *POBJECT_HANDLE_FLAG_INFORMATION;

typedef struct _SYSTEM_OBJECT_INFORMATION // Size=48
{
	ULONG NextEntryOffset; // Size=4 Offset=0
	PVOID Object; // Size=4 Offset=4
	PVOID CreatorUniqueProcess; // Size=4 Offset=8
	USHORT CreatorBackTraceIndex; // Size=2 Offset=12
	USHORT Flags; // Size=2 Offset=14
	LONG PointerCount; // Size=4 Offset=16
	LONG HandleCount; // Size=4 Offset=20
	ULONG PagedPoolCharge; // Size=4 Offset=24
	ULONG NonPagedPoolCharge; // Size=4 Offset=28
	PVOID ExclusiveProcessId; // Size=4 Offset=32
	PVOID SecurityDescriptor; // Size=4 Offset=36
	OBJECT_NAME_INFORMATION NameInfo; // Size=8 Offset=40
} SYSTEM_OBJECT_INFORMATION, *PSYSTEM_OBJECT_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE
{
	DWORD    dwProcessId;
	BYTE     bObjectType;
	BYTE     bFlags;
	WORD     wValue;
	PVOID    pAddress;
	DWORD    GrantedAccess;
}SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG NumberOfHandles;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;



//NTSTATUS NTAPI NtQueryObject(
//	_In_opt_   HANDLE Handle,
//	_In_       OBJECT_INFORMATION_CLASS ObjectInformationClass,
//	_Out_opt_  PVOID ObjectInformation,
//	_In_       ULONG ObjectInformationLength,
//	_Out_opt_  PULONG ReturnLength
//);
//
//NTSTATUS WINAPI NtQuerySystemInformation(
//	_In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
//	_Inout_    PVOID SystemInformation,
//	_In_       ULONG SystemInformationLength,
//	_Out_opt_  PULONG ReturnLength
//);

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

typedef std::function<NTSTATUS(PSYSTEM_HANDLE_TABLE_ENTRY_INFO)> ENUM_HANDLE_CALLBACK;

bool EnablePrivilege(
	LPCWSTR lpPrivilegeName
);

bool DisablePrivilege(
	LPCWSTR lpPrivilegeName
);

NTSTATUS EnumerateHandles(ENUM_HANDLE_CALLBACK callback);
HANDLE GetHandleToProcess(LPWSTR process);
void PrintSelfToOthersHandles();
