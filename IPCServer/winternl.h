/************************************************************************
*                                                                       *
*   winternl.h -- This module defines the internal NT APIs and data     *
*       structures that are intended for the use only by internal core  *
*       Windows components.  These APIs and data structures may change  *
*       at any time.                                                    *
*                                                                       *
*   These APIs and data structures are subject to changes from one      *
*       Windows release to another Windows release.  To maintain the    *
*       compatiblity of your application, avoid using these APIs and    *
*       data structures.                                                *
*                                                                       *
*   The appropriate mechanism for accessing the functions defined in    *
*       this header is to use LoadLibrary() for ntdll.dll and           *
*       GetProcAddress() for the particular function.  By using this    *
*       approach, your application will be more resilient to changes    *
*       for these functions between Windows releases.  If a function    *
*       prototype does change, then GetProcAddress() for that function  *
*       might detect the change and fail the function call, which your  *
*       application will be able to detect.  GetProcAddress() may not   *
*       be able to detect all signature changes, thus avoid using these *
*       internal functions.  Instead, your application should use the   *
*       appropriate Win32 function that provides equivalent or similiar *
*       functionality.                                                  *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/

#ifndef _WINTERNL_
#define _WINTERNL_
#include <winapifamily.h>

#pragma region Desktop Family
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)


#if (_WIN32_WINNT >= 0x0500)

#include <windef.h>

#ifdef __cplusplus
extern "C" {
#endif

	//
	// These data structures and type definitions are needed for compilation and
	// use of the internal Windows APIs defined in this header.
	//

	typedef _Return_type_success_(return >= 0) LONG NTSTATUS;

	typedef CONST char *PCSZ;

	typedef struct _STRING {
		USHORT Length;
		USHORT MaximumLength;
		PCHAR Buffer;
	} STRING;
	typedef STRING *PSTRING;

	typedef STRING ANSI_STRING;
	typedef PSTRING PANSI_STRING;
	typedef PSTRING PCANSI_STRING;

	typedef STRING OEM_STRING;
	typedef PSTRING POEM_STRING;
	typedef CONST STRING* PCOEM_STRING;

	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING;
	typedef UNICODE_STRING *PUNICODE_STRING;
	typedef const UNICODE_STRING *PCUNICODE_STRING;

	//
	// The PEB_LDR_DATA, LDR_DATA_TABLE_ENTRY, RTL_USER_PROCESS_PARAMETERS, PEB
	// and TEB structures are subject to changes between Windows releases; thus,
	// the field offsets and reserved fields may change. The reserved fields are
	// reserved for use only by the Windows operating systems. Do not assume a
	// maximum size for these structures.
	//
	// Instead of using the InMemoryOrderModuleList field of the
	//     LDR_DATA_TABLE_ENTRY structure, use the Win32 API EnumProcessModules
	//
	// Instead of using the IsBeingDebugged field of the PEB structure, use the
	//     Win32 APIs IsDebuggerPresent or CheckRemoteDebuggerPresent
	//
	// Instead of using the SessionId field of the PEB structure, use the Win32
	//     APIs GetCurrentProcessId and ProcessIdToSessionId
	//
	// Instead of using the Tls fields of the TEB structure, use the Win32 APIs
	//     TlsAlloc, TlsGetValue, TlsSetValue and TlsFree
	//
	// Instead of using the ReservedForOle field, use the COM API
	//     CoGetContextToken
	//
	// Sample x86 assembly code that gets the SessionId (subject to change
	//     between Windows releases, use the Win32 APIs to make your application
	//     resilient to changes)
	//     mov     eax,fs:[00000018]
	//     mov     eax,[eax+0x30]
	//     mov     eax,[eax+0x1d4]
	//

	//
	// N.B. Fields marked as reserved do not necessarily reflect the structure
	//      of the real struct. They may simply guarantee that the offets of 
	//      the exposed fields are correct. When code matches this pattern,
	//
	//          TYPE1 ExposedField1;
	//          BYTE ReservedBytes[b];
	//          PVOID ReservedPtrs[p];
	//          TYPE2 ExposedField2;
	//
	//      or that pattern with ReservedBytes and ReservedPtrs swapped, it is
	//      likely that 'b' and 'p' are derived from the following system:
	//
	//          GapThirtyTwo = 4p + b
	//          GapSixtyFour = 8p + b
	//      
	//      where GapThirtyTwo is the number of bytes between the two exposed
	//      fields in the 32-bit version of the real struct and GapSixtyFour
	//      is the number of bytes between the two exposed fields in the 64-bit
	//      version of the real struct.
	//
	//      Also note that such code must take into account the alignment of
	//      the ReservedPtrs field.
	//

	typedef struct _PEB_LDR_DATA {
		BYTE Reserved1[8];
		PVOID Reserved2[3];
		LIST_ENTRY InMemoryOrderModuleList;
	} PEB_LDR_DATA, *PPEB_LDR_DATA;

	typedef struct _LDR_DATA_TABLE_ENTRY {
		PVOID Reserved1[2];
		LIST_ENTRY InMemoryOrderLinks;
		PVOID Reserved2[2];
		PVOID DllBase;
		PVOID Reserved3[2];
		UNICODE_STRING FullDllName;
		BYTE Reserved4[8];
		PVOID Reserved5[3];
		union {
			ULONG CheckSum;
			PVOID Reserved6;
		} DUMMYUNIONNAME;
		ULONG TimeDateStamp;
	} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		BYTE Reserved1[16];
		PVOID Reserved2[10];
		UNICODE_STRING ImagePathName;
		UNICODE_STRING CommandLine;
	} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

	typedef
		VOID
		(NTAPI *PPS_POST_PROCESS_INIT_ROUTINE) (
			VOID
			);

	typedef struct _PEB {
		BYTE Reserved1[2];
		BYTE BeingDebugged;
		BYTE Reserved2[1];
		PVOID Reserved3[2];
		PPEB_LDR_DATA Ldr;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID Reserved4[3];
		PVOID AtlThunkSListPtr;
		PVOID Reserved5;
		ULONG Reserved6;
		PVOID Reserved7;
		ULONG Reserved8;
		ULONG AtlThunkSListPtr32;
		PVOID Reserved9[45];
		BYTE Reserved10[96];
		PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
		BYTE Reserved11[128];
		PVOID Reserved12[1];
		ULONG SessionId;
	} PEB, *PPEB;

	typedef struct _TEB {
		PVOID Reserved1[12];
		PPEB ProcessEnvironmentBlock;
		PVOID Reserved2[399];
		BYTE Reserved3[1952];
		PVOID TlsSlots[64];
		BYTE Reserved4[8];
		PVOID Reserved5[26];
		PVOID ReservedForOle;  // Windows 2000 only
		PVOID Reserved6[4];
		PVOID TlsExpansionSlots;
	} TEB, *PTEB;

	typedef struct _OBJECT_ATTRIBUTES {
		ULONG Length;
		HANDLE RootDirectory;
		PUNICODE_STRING ObjectName;
		ULONG Attributes;
		PVOID SecurityDescriptor;
		PVOID SecurityQualityOfService;
	} OBJECT_ATTRIBUTES;
	typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

	typedef struct _IO_STATUS_BLOCK {
		union {
			NTSTATUS Status;
			PVOID Pointer;
		} DUMMYUNIONNAME;

		ULONG_PTR Information;
	} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

	typedef
		VOID
		(NTAPI *PIO_APC_ROUTINE) (
			IN PVOID ApcContext,
			IN PIO_STATUS_BLOCK IoStatusBlock,
			IN ULONG Reserved
			);

	typedef struct _PROCESS_BASIC_INFORMATION {
		PVOID Reserved1;
		PPEB PebBaseAddress;
		PVOID Reserved2[2];
		ULONG_PTR UniqueProcessId;
		PVOID Reserved3;
	} PROCESS_BASIC_INFORMATION;
	typedef PROCESS_BASIC_INFORMATION *PPROCESS_BASIC_INFORMATION;

	typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
		LARGE_INTEGER IdleTime;
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER Reserved1[2];
		ULONG Reserved2;
	} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

	typedef struct _SYSTEM_PROCESS_INFORMATION {
		ULONG NextEntryOffset;
		BYTE Reserved1[52];
		PVOID Reserved2[3];
		HANDLE UniqueProcessId;
		PVOID Reserved3;
		ULONG HandleCount;
		BYTE Reserved4[4];
		PVOID Reserved5[11];
		SIZE_T PeakPagefileUsage;
		SIZE_T PrivatePageCount;
		LARGE_INTEGER Reserved6[6];
	} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

	typedef struct _SYSTEM_REGISTRY_QUOTA_INFORMATION {
		ULONG RegistryQuotaAllowed;
		ULONG RegistryQuotaUsed;
		PVOID Reserved1;
	} SYSTEM_REGISTRY_QUOTA_INFORMATION, *PSYSTEM_REGISTRY_QUOTA_INFORMATION;

	typedef struct _SYSTEM_BASIC_INFORMATION {
		BYTE Reserved1[24];
		PVOID Reserved2[4];
		CCHAR NumberOfProcessors;
	} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

	typedef struct _SYSTEM_TIMEOFDAY_INFORMATION {
		BYTE Reserved1[48];
	} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

	typedef struct _SYSTEM_PERFORMANCE_INFORMATION {
		BYTE Reserved1[312];
	} SYSTEM_PERFORMANCE_INFORMATION, *PSYSTEM_PERFORMANCE_INFORMATION;

	typedef struct _SYSTEM_EXCEPTION_INFORMATION {
		BYTE Reserved1[16];
	} SYSTEM_EXCEPTION_INFORMATION, *PSYSTEM_EXCEPTION_INFORMATION;

	typedef struct _SYSTEM_LOOKASIDE_INFORMATION {
		BYTE Reserved1[32];
	} SYSTEM_LOOKASIDE_INFORMATION, *PSYSTEM_LOOKASIDE_INFORMATION;

	typedef struct _SYSTEM_INTERRUPT_INFORMATION {
		BYTE Reserved1[24];
	} SYSTEM_INTERRUPT_INFORMATION, *PSYSTEM_INTERRUPT_INFORMATION;

	typedef struct _SYSTEM_POLICY_INFORMATION {
		PVOID Reserved1[2];
		ULONG Reserved2[3];
	} SYSTEM_POLICY_INFORMATION, *PSYSTEM_POLICY_INFORMATION;

	typedef enum _FILE_INFORMATION_CLASS {
		FileDirectoryInformation = 1
	} FILE_INFORMATION_CLASS;

	typedef enum _PROCESSINFOCLASS {
		ProcessBasicInformation = 0,
		ProcessDebugPort = 7,
		ProcessWow64Information = 26,
		ProcessImageFileName = 27,
		ProcessBreakOnTermination = 29
	} PROCESSINFOCLASS;

	typedef enum _THREADINFOCLASS {
		ThreadIsIoPending = 16
	} THREADINFOCLASS;

	typedef enum _SYSTEM_INFORMATION_CLASS
	{
		SystemBasicInformation = 0x0000,
		SystemProcessorInformation = 0x0001,
		SystemPerformanceInformation = 0x0002,
		SystemTimeOfDayInformation = 0x0003,
		SystemPathInformation = 0x0004,
		SystemProcessInformation = 0x0005,
		SystemCallCountInformation = 0x0006,
		SystemDeviceInformation = 0x0007,
		SystemProcessorPerformanceInformation = 0x0008,
		SystemFlagsInformation = 0x0009,
		SystemCallTimeInformation = 0x000A,
		SystemModuleInformation = 0x000B,
		SystemLocksInformation = 0x000C,
		SystemStackTraceInformation = 0x000D,
		SystemPagedPoolInformation = 0x000E,
		SystemNonPagedPoolInformation = 0x000F,
		SystemHandleInformation = 0x0010,
		SystemObjectInformation = 0x0011,
		SystemPageFileInformation = 0x0012,
		SystemVdmInstemulInformation = 0x0013,
		SystemVdmBopInformation = 0x0014,
		SystemFileCacheInformation = 0x0015,
		SystemPoolTagInformation = 0x0016,
		SystemInterruptInformation = 0x0017,
		SystemDpcBehaviorInformation = 0x0018,
		SystemFullMemoryInformation = 0x0019,
		SystemLoadGdiDriverInformation = 0x001A,
		SystemUnloadGdiDriverInformation = 0x001B,
		SystemTimeAdjustmentInformation = 0x001C,
		SystemSummaryMemoryInformation = 0x001D,
		SystemMirrorMemoryInformation = 0x001E,
		SystemPerformanceTraceInformation = 0x001F,
		SystemCrashDumpInformation = 0x0020,
		SystemExceptionInformation = 0x0021,
		SystemCrashDumpStateInformation = 0x0022,
		SystemKernelDebuggerInformation = 0x0023,
		SystemContextSwitchInformation = 0x0024,
		SystemRegistryQuotaInformation = 0x0025,
		SystemExtendServiceTableInformation = 0x0026,
		SystemPrioritySeperation = 0x0027,
		SystemVerifierAddDriverInformation = 0x0028,
		SystemVerifierRemoveDriverInformation = 0x0029,
		SystemProcessorIdleInformation = 0x002A,
		SystemLegacyDriverInformation = 0x002B,
		SystemCurrentTimeZoneInformation = 0x002C,
		SystemLookasideInformation = 0x002D,
		SystemTimeSlipNotification = 0x002E,
		SystemSessionCreate = 0x002F,
		SystemSessionDetach = 0x0030,
		SystemSessionInformation = 0x0031,
		SystemRangeStartInformation = 0x0032,
		SystemVerifierInformation = 0x0033,
		SystemVerifierThunkExtend = 0x0034,
		SystemSessionProcessInformation = 0x0035,
		SystemLoadGdiDriverInSystemSpace = 0x0036,
		SystemNumaProcessorMap = 0x0037,
		SystemPrefetcherInformation = 0x0038,
		SystemExtendedProcessInformation = 0x0039,
		SystemRecommendedSharedDataAlignment = 0x003A,
		SystemComPlusPackage = 0x003B,
		SystemNumaAvailableMemory = 0x003C,
		SystemProcessorPowerInformation = 0x003D,
		SystemEmulationBasicInformation = 0x003E,
		SystemEmulationProcessorInformation = 0x003F,
		SystemExtendedHandleInformation = 0x0040,
		SystemLostDelayedWriteInformation = 0x0041,
		SystemBigPoolInformation = 0x0042,
		SystemSessionPoolTagInformation = 0x0043,
		SystemSessionMappedViewInformation = 0x0044,
		SystemHotpatchInformation = 0x0045,
		SystemObjectSecurityMode = 0x0046,
		SystemWatchdogTimerHandler = 0x0047,
		SystemWatchdogTimerInformation = 0x0048,
		SystemLogicalProcessorInformation = 0x0049,
		SystemWow64SharedInformationObsolete = 0x004A,
		SystemRegisterFirmwareTableInformationHandler = 0x004B,
		SystemFirmwareTableInformation = 0x004C,
		SystemModuleInformationEx = 0x004D,
		SystemVerifierTriageInformation = 0x004E,
		SystemSuperfetchInformation = 0x004F,
		SystemMemoryListInformation = 0x0050,
		SystemFileCacheInformationEx = 0x0051,
		SystemThreadPriorityClientIdInformation = 0x0052,
		SystemProcessorIdleCycleTimeInformation = 0x0053,
		SystemVerifierCancellationInformation = 0x0054,
		SystemProcessorPowerInformationEx = 0x0055,
		SystemRefTraceInformation = 0x0056,
		SystemSpecialPoolInformation = 0x0057,
		SystemProcessIdInformation = 0x0058,
		SystemErrorPortInformation = 0x0059,
		SystemBootEnvironmentInformation = 0x005A,
		SystemHypervisorInformation = 0x005B,
		SystemVerifierInformationEx = 0x005C,
		SystemTimeZoneInformation = 0x005D,
		SystemImageFileExecutionOptionsInformation = 0x005E,
		SystemCoverageInformation = 0x005F,
		SystemPrefetchPatchInformation = 0x0060,
		SystemVerifierFaultsInformation = 0x0061,
		SystemSystemPartitionInformation = 0x0062,
		SystemSystemDiskInformation = 0x0063,
		SystemProcessorPerformanceDistribution = 0x0064,
		SystemNumaProximityNodeInformation = 0x0065,
		SystemDynamicTimeZoneInformation = 0x0066,
		SystemCodeIntegrityInformation = 0x0067,
		SystemProcessorMicrocodeUpdateInformation = 0x0068,
		SystemProcessorBrandString = 0x0069,
		SystemVirtualAddressInformation = 0x006A,
		SystemLogicalProcessorAndGroupInformation = 0x006B,
		SystemProcessorCycleTimeInformation = 0x006C,
		SystemStoreInformation = 0x006D,
		SystemRegistryAppendString = 0x006E,
		SystemAitSamplingValue = 0x006F,
		SystemVhdBootInformation = 0x0070,
		SystemCpuQuotaInformation = 0x0071,
		SystemNativeBasicInformation = 0x0072,
		SystemErrorPortTimeouts = 0x0073,
		SystemLowPriorityIoInformation = 0x0074,
		SystemBootEntropyInformation = 0x0075,
		SystemVerifierCountersInformation = 0x0076,
		SystemPagedPoolInformationEx = 0x0077,
		SystemSystemPtesInformationEx = 0x0078,
		SystemNodeDistanceInformation = 0x0079,
		SystemAcpiAuditInformation = 0x007A,
		SystemBasicPerformanceInformation = 0x007B,
		SystemQueryPerformanceCounterInformation = 0x007C,
		SystemSessionBigPoolInformation = 0x007D,
		SystemBootGraphicsInformation = 0x007E,
		SystemScrubPhysicalMemoryInformation = 0x007F,
		SystemBadPageInformation = 0x0080,
		SystemProcessorProfileControlArea = 0x0081,
		SystemCombinePhysicalMemoryInformation = 0x0082,
		SystemEntropyInterruptTimingInformation = 0x0083,
		SystemConsoleInformation = 0x0084,
		SystemPlatformBinaryInformation = 0x0085,
		SystemThrottleNotificationInformation = 0x0086,
		SystemHypervisorProcessorCountInformation = 0x0087,
		SystemDeviceDataInformation = 0x0088,
		SystemDeviceDataEnumerationInformation = 0x0089,
		SystemMemoryTopologyInformation = 0x008A,
		SystemMemoryChannelInformation = 0x008B,
		SystemBootLogoInformation = 0x008C,
		SystemProcessorPerformanceInformationEx = 0x008D,
		SystemSpare0 = 0x008E,
		SystemSecureBootPolicyInformation = 0x008F,
		SystemPageFileInformationEx = 0x0090,
		SystemSecureBootInformation = 0x0091,
		SystemEntropyInterruptTimingRawInformation = 0x0092,
		SystemPortableWorkspaceEfiLauncherInformation = 0x0093,
		SystemFullProcessInformation = 0x0094,
		MaxSystemInfoClass = 0x0095
	} SYSTEM_INFORMATION_CLASS;

	//
	// Object Information Classes
	//

	typedef enum _OBJECT_INFORMATION_CLASS
	{
		ObjectBasicInformation,
		ObjectNameInformation,
		ObjectTypeInformation,
		ObjectTypesInformation,
		ObjectHandleFlagInformation,
		ObjectSessionInformation,
		MaxObjectInfoClass
	} OBJECT_INFORMATION_CLASS;

	//
	//  Public Object Information definitions
	//

	typedef struct _PUBLIC_OBJECT_BASIC_INFORMATION {
		ULONG Attributes;
		ACCESS_MASK GrantedAccess;
		ULONG HandleCount;
		ULONG PointerCount;

		ULONG Reserved[10];    // reserved for internal use

	} PUBLIC_OBJECT_BASIC_INFORMATION, *PPUBLIC_OBJECT_BASIC_INFORMATION;

	typedef struct __PUBLIC_OBJECT_TYPE_INFORMATION {

		UNICODE_STRING TypeName;

		ULONG Reserved[22];    // reserved for internal use

	} PUBLIC_OBJECT_TYPE_INFORMATION, *PPUBLIC_OBJECT_TYPE_INFORMATION;

#if (_WIN32_WINNT >= 0x0501)
	//
	// use the WTS API instead
	//     WTSGetActiveConsoleSessionId
	// The active console id is cached as a volatile ULONG in a constant
	// memory location.  This x86 memory location is subject to changes between
	// Windows releases.  Use the WTS API to make your application resilient to
	// changes.
	//
#define INTERNAL_TS_ACTIVE_CONSOLE_ID ( *((volatile ULONG*)(0x7ffe02d8)) )
#endif // (_WIN32_WINNT >= 0x0501)

	//
	// These functions are intended for use by internal core Windows components
	// since these functions may change between Windows releases.
	//

#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

	//
	// use the Win32 API instead
	//     CloseHandle
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtClose(
			IN HANDLE Handle
		);

	//
	// use the Win32 API instead
	//     CreateFile
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtCreateFile(
			OUT PHANDLE FileHandle,
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

	//
	// use the Win32 API instead
	//     CreateFile
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtOpenFile(
			OUT PHANDLE FileHandle,
			IN ACCESS_MASK DesiredAccess,
			IN POBJECT_ATTRIBUTES ObjectAttributes,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			IN ULONG ShareAccess,
			IN ULONG OpenOptions
		);

	//
	// use the Win32 API instead
	//     N/A
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtRenameKey(
			_In_ HANDLE KeyHandle,
			_In_ PUNICODE_STRING NewName
		);

	//
	// use the Win32 API instead
	//     RegNotifyChangeKeyValue
	//

	__kernel_entry NTSTATUS
		NTAPI
		NtNotifyChangeMultipleKeys(
			_In_ HANDLE MasterKeyHandle,
			_In_opt_ ULONG Count,
			_In_reads_opt_(Count) OBJECT_ATTRIBUTES SubordinateObjects[],
			_In_opt_ HANDLE Event,
			_In_opt_ PIO_APC_ROUTINE ApcRoutine,
			_In_opt_ PVOID ApcContext,
			_Out_ PIO_STATUS_BLOCK IoStatusBlock,
			_In_ ULONG CompletionFilter,
			_In_ BOOLEAN WatchTree,
			_Out_writes_bytes_opt_(BufferSize) PVOID Buffer,
			_In_ ULONG BufferSize,
			_In_ BOOLEAN Asynchronous
		);

	//
	// use the Win32 API instead
	//     RegQueryValueEx
	//

	typedef struct _KEY_VALUE_ENTRY {
		PUNICODE_STRING ValueName;
		ULONG           DataLength;
		ULONG           DataOffset;
		ULONG           Type;
	} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

	__kernel_entry NTSTATUS
		NTAPI
		NtQueryMultipleValueKey(
			_In_ HANDLE KeyHandle,
			_Inout_updates_(EntryCount) PKEY_VALUE_ENTRY ValueEntries,
			_In_ ULONG EntryCount,
			_Out_writes_bytes_(*BufferLength) PVOID ValueBuffer,
			_Inout_ PULONG BufferLength,
			_Out_opt_ PULONG RequiredBufferLength
		);

	//
	// use the Win32 API instead
	//     N/A
	//

	typedef enum _KEY_SET_INFORMATION_CLASS {
		KeyWriteTimeInformation,
		KeyWow64FlagsInformation,
		KeyControlFlagsInformation,
		KeySetVirtualizationInformation,
		KeySetDebugInformation,
		KeySetHandleTagsInformation,
		MaxKeySetInfoClass  // MaxKeySetInfoClass should always be the last enum
	} KEY_SET_INFORMATION_CLASS;

	__kernel_entry NTSTATUS
		NTAPI
		NtSetInformationKey(
			_In_ HANDLE KeyHandle,
			_In_ _Strict_type_match_
			KEY_SET_INFORMATION_CLASS KeySetInformationClass,
			_In_reads_bytes_(KeySetInformationLength) PVOID KeySetInformation,
			_In_ ULONG KeySetInformationLength
		);

	//
	// use the Win32 API instead
	//     DeviceIoControl
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtDeviceIoControlFile(
			IN HANDLE FileHandle,
			IN HANDLE Event OPTIONAL,
			IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
			IN PVOID ApcContext OPTIONAL,
			OUT PIO_STATUS_BLOCK IoStatusBlock,
			IN ULONG IoControlCode,
			IN PVOID InputBuffer OPTIONAL,
			IN ULONG InputBufferLength,
			OUT PVOID OutputBuffer OPTIONAL,
			IN ULONG OutputBufferLength
		);

	//
	// use the Win32 API instead
	//     WaitForSingleObjectEx
	//
	NTSTATUS
		NTAPI
		NtWaitForSingleObject(
			IN HANDLE Handle,
			IN BOOLEAN Alertable,
			IN PLARGE_INTEGER Timeout OPTIONAL
		);

	//
	// use the Win32 API instead
	//     CheckNameLegalDOS8Dot3
	//
	BOOLEAN
		NTAPI
		RtlIsNameLegalDOS8Dot3(
			IN PUNICODE_STRING Name,
			IN OUT POEM_STRING OemName OPTIONAL,
			IN OUT PBOOLEAN NameContainsSpaces OPTIONAL
		);

	//
	// This function might be needed for some of the internal Windows functions,
	// defined in this header file.
	//
	_When_(Status < 0, _Out_range_(>, 0))
		_When_(Status >= 0, _Out_range_(== , 0))
		ULONG
		NTAPI
		RtlNtStatusToDosError(
			NTSTATUS Status
		);

	//
	// use the Win32 APIs instead
	//     GetProcessHandleCount
	//     GetProcessId
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtQueryInformationProcess(
			IN HANDLE ProcessHandle,
			IN PROCESSINFOCLASS ProcessInformationClass,
			OUT PVOID ProcessInformation,
			IN ULONG ProcessInformationLength,
			OUT PULONG ReturnLength OPTIONAL
		);

	//
	// use the Win32 API instead
	//     GetThreadIOPendingFlag
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtQueryInformationThread(
			IN HANDLE ThreadHandle,
			IN THREADINFOCLASS ThreadInformationClass,
			OUT PVOID ThreadInformation,
			IN ULONG ThreadInformationLength,
			OUT PULONG ReturnLength OPTIONAL
		);

	//
	// use the Win32 APIs instead
	//     GetFileInformationByHandle
	//     GetFileInformationByHandleEx
	//     GetProcessInformation
	//     GetThreadInformation
	//     

	__kernel_entry NTSYSCALLAPI
		NTSTATUS
		NTAPI
		NtQueryObject(
			_In_opt_ HANDLE Handle,
			_In_ OBJECT_INFORMATION_CLASS ObjectInformationClass,
			_Out_writes_bytes_opt_(ObjectInformationLength) PVOID ObjectInformation,
			_In_ ULONG ObjectInformationLength,
			_Out_opt_ PULONG ReturnLength
		);

	//
	// use the Win32 APIs instead
	//     GetSystemRegistryQuota
	//     GetSystemTimes
	// use the CryptoAPIs instead for generating random data
	//     CryptGenRandom
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtQuerySystemInformation(
			IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
			OUT PVOID SystemInformation,
			IN ULONG SystemInformationLength,
			OUT PULONG ReturnLength OPTIONAL
		);

	//
	// use the Win32 API instead
	//     GetSystemTimeAsFileTime
	//
	__kernel_entry NTSTATUS
		NTAPI
		NtQuerySystemTime(
			OUT PLARGE_INTEGER SystemTime
		);

	//
	// use the Win32 API instead
	//     LocalFileTimeToFileTime
	//
	NTSTATUS
		NTAPI
		RtlLocalTimeToSystemTime(
			IN PLARGE_INTEGER LocalTime,
			OUT PLARGE_INTEGER SystemTime
		);

	//
	// use the Win32 API instead
	//     SystemTimeToFileTime to convert to FILETIME structures
	//     copy the resulting FILETIME structures to ULARGE_INTEGER structures
	//     perform the calculation
	//
	BOOLEAN
		NTAPI
		RtlTimeToSecondsSince1970(
			PLARGE_INTEGER Time,
			PULONG ElapsedSeconds
		);

	//
	// These APIs might be need for some of the internal Windows functions,
	// defined in this header file.
	//
	VOID
		NTAPI
		RtlFreeAnsiString(
			PANSI_STRING AnsiString
		);

	VOID
		NTAPI
		RtlFreeUnicodeString(
			PUNICODE_STRING UnicodeString
		);

	VOID
		NTAPI
		RtlFreeOemString(
			POEM_STRING OemString
		);

	VOID
		NTAPI
		RtlInitString(
			PSTRING DestinationString,
			PCSZ SourceString
		);

	VOID
		NTAPI
		RtlInitAnsiString(
			PANSI_STRING DestinationString,
			PCSZ SourceString
		);

	VOID
		NTAPI
		RtlInitUnicodeString(
			PUNICODE_STRING DestinationString,
			PCWSTR SourceString
		);

	NTSTATUS
		NTAPI
		RtlAnsiStringToUnicodeString(
			PUNICODE_STRING DestinationString,
			PCANSI_STRING SourceString,
			BOOLEAN AllocateDestinationString
		);

	NTSTATUS
		NTAPI
		RtlUnicodeStringToAnsiString(
			PANSI_STRING DestinationString,
			PCUNICODE_STRING SourceString,
			BOOLEAN AllocateDestinationString
		);

	NTSTATUS
		NTAPI
		RtlUnicodeStringToOemString(
			POEM_STRING DestinationString,
			PCUNICODE_STRING SourceString,
			BOOLEAN AllocateDestinationString
		);

	//
	// Use the Win32 API instead
	//     WideCharToMultiByte
	//     set CodePage to CP_ACP
	//     set cbMultiByte to 0
	//
	NTSTATUS
		NTAPI
		RtlUnicodeToMultiByteSize(
			_Out_ PULONG BytesInMultiByteString,
			_In_reads_bytes_(BytesInUnicodeString) PWCH UnicodeString,
			_In_ ULONG BytesInUnicodeString
		);

	//
	// Use the C runtime function instead
	//     strtol
	//
	NTSTATUS
		NTAPI
		RtlCharToInteger(
			PCSZ String,
			ULONG Base,
			PULONG Value
		);

	//
	// use the Win32 API instead
	//     ConvertSidToStringSid
	//
	NTSTATUS
		NTAPI
		RtlConvertSidToUnicodeString(
			PUNICODE_STRING UnicodeString,
			PSID Sid,
			BOOLEAN AllocateDestinationString
		);

	//
	// use the CryptoAPIs instead
	//     CryptGenRandom
	//
	ULONG
		NTAPI
		RtlUniform(
			PULONG Seed
		);


#define LOGONID_CURRENT     ((ULONG)-1)
#define SERVERNAME_CURRENT  ((HANDLE)NULL)

	typedef enum _WINSTATIONINFOCLASS {
		WinStationInformation = 8
	} WINSTATIONINFOCLASS;


	typedef struct _WINSTATIONINFORMATIONW {
		BYTE Reserved2[70];
		ULONG LogonId;
		BYTE Reserved3[1140];
	} WINSTATIONINFORMATIONW, *PWINSTATIONINFORMATIONW;

	//
	// this function is implemented in winsta.dll (you need to loadlibrary to call this function)
	// this internal function retrives the LogonId (also called SessionId) for the current process
	// You should avoid using this function as it can change. you can retrieve the same information 
	// Using public api WTSQuerySessionInformation. Pass WTSSessionId as the WTSInfoClass parameter
	// 
	typedef BOOLEAN(WINAPI * PWINSTATIONQUERYINFORMATIONW)(
		HANDLE, ULONG, WINSTATIONINFOCLASS, PVOID, ULONG, PULONG);

	//
	// Generic test for success on any status value (non-negative numbers
	// indicate success).
	//

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

	//
	// Generic test for information on any status value.
	//

#ifndef NT_INFORMATION
#define NT_INFORMATION(Status) ((((ULONG)(Status)) >> 30) == 1)
#endif

	//
	// Generic test for warning on any status value.
	//

#ifndef NT_WARNING
#define NT_WARNING(Status) ((((ULONG)(Status)) >> 30) == 2)
#endif

	//
	// Generic test for error on any status value.
	//

#ifndef NT_ERROR
#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)
#endif

	//++
	//
	// VOID
	// InitializeObjectAttributes(
	//     OUT POBJECT_ATTRIBUTES p,
	//     IN PUNICODE_STRING n,
	//     IN ULONG a,
	//     IN HANDLE r,
	//     IN PSECURITY_DESCRIPTOR s
	//     )
	//
	//--

#ifndef InitializeObjectAttributes
#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }
#endif

	//
	// Valid values for the Attributes field
	//

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#define OBJ_VALID_ATTRIBUTES    0x000007F2L

	//
	// Define the create disposition values
	//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

	//
	// Define the create/open option flags
	//

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_OPEN_REMOTE_INSTANCE               0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN7)
#define FILE_OPEN_REQUIRING_OPLOCK              0x00010000
#endif

#define FILE_RESERVE_OPFILTER                   0x00100000
#define FILE_OPEN_REPARSE_POINT                 0x00200000
#define FILE_OPEN_NO_RECALL                     0x00400000
#define FILE_OPEN_FOR_FREE_SPACE_QUERY          0x00800000

#define FILE_VALID_OPTION_FLAGS                 0x00ffffff
#define FILE_VALID_PIPE_OPTION_FLAGS            0x00000032
#define FILE_VALID_MAILSLOT_OPTION_FLAGS        0x00000032
#define FILE_VALID_SET_FLAGS                    0x00000036

	//
	// Define the I/O status information return values for NtCreateFile/NtOpenFile
	//

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

#ifdef __cplusplus
}
#endif

#endif // (_WIN32_WINNT >= 0x0500)


#endif /* WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) */
#pragma endregion

#endif // _WINTERNL_
