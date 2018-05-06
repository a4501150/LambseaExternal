#pragma once
#include "stdafx.h"

#define ReCa reinterpret_cast

typedef NTSTATUS(__stdcall * f_NtCreateThreadEx)(HANDLE * pHandle, ACCESS_MASK DesiredAccess, void * pAttr, HANDLE hProc, void * pFunc, void * pArg,
	ULONG Flags, SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaxStackSize, void * pAttrListOut);

struct UNICODE_STRING
{
	WORD		Length;
	WORD		MaxLength;
	wchar_t *	szBuffer;
};

struct LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY		InLoadOrder;
	LIST_ENTRY		InMemoryOrder;
	LIST_ENTRY		InInitOrder;
	void *			DllBase;
	void *			EntryPoint;
	ULONG			SizeOfImage;
	UNICODE_STRING	FullDllName;
	UNICODE_STRING	BaseDllName;
};
typedef NTSTATUS(__stdcall * f_LdrLoadDll)(wchar_t * szOptPath, ULONG ulFlags, UNICODE_STRING * pModuleFileName, HANDLE * pOut);
struct LDR_LOAD_DLL_DATA
{
	f_LdrLoadDll	pLdrLoadDll;
	HANDLE			Out;
	UNICODE_STRING	pModuleFileName;
	BYTE			Data[MAX_PATH * 2];
};

bool SetPrivilegeA(const char * szPrivilege, bool bState = true)
{
	HANDLE hToken = nullptr;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		return false;
	}

	TOKEN_PRIVILEGES TokenPrivileges = { 0 };
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bState ? SE_PRIVILEGE_ENABLED : 0;

	if (!LookupPrivilegeValueA(nullptr, szPrivilege, &TokenPrivileges.Privileges[0].Luid))
	{
		CloseHandle(hToken);
		return false;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
	{
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);

	return true;
}

bool FileExistsA(const char * szFile)
{
	return (GetFileAttributesA(szFile) != INVALID_FILE_ATTRIBUTES);
}

void __stdcall LdrLoadDllShell(LDR_LOAD_DLL_DATA * pData)
{
	if (!pData)
		return;

	pData->pModuleFileName.szBuffer = ReCa<wchar_t*>(pData->Data);
	pData->pLdrLoadDll(nullptr, 0, &pData->pModuleFileName, &pData->Out);
}

UINT __forceinline _strlenA(const char * szString)
{
	UINT Ret = 0;
	for (; *szString++; Ret++);
	return Ret;
}

HANDLE StartRoutine(HANDLE hTargetProc, void * pRoutine, void * pArg)
{
	auto _NtCTE = reinterpret_cast<f_NtCreateThreadEx>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx"));
	if (!_NtCTE)
	{
		HANDLE hThread = CreateRemoteThreadEx(hTargetProc, nullptr, 0, ReCa<LPTHREAD_START_ROUTINE>(pRoutine), pArg, 0, nullptr, nullptr);
		if (!hThread)
			return INVALID_HANDLE_VALUE;

		return hThread;
	}

	HANDLE hThread = nullptr;
	_NtCTE(&hThread, THREAD_ALL_ACCESS, nullptr, hTargetProc, pRoutine, pArg, 0, 0, 0, 0, nullptr);

	if (!hThread)
		return INVALID_HANDLE_VALUE;

	return hThread;
}

DWORD LdrLoadDllStub(const char * szDllFile, HANDLE hProc)
{
	if (!hProc)
		return GetLastError();
	if (!szDllFile || !FileExistsA(szDllFile))
		return -1;

	LDR_LOAD_DLL_DATA data{ 0 };
	data.pModuleFileName.szBuffer = ReCa<wchar_t*>(data.Data);
	data.pModuleFileName.MaxLength = MAX_PATH * 2;

	size_t len = _strlenA(szDllFile);
	mbstowcs_s(&len, data.pModuleFileName.szBuffer, len + 1, szDllFile, len);
	data.pModuleFileName.Length = (WORD)(len * 2) - 2;

	HINSTANCE hNTDLL = GetModuleHandleA("ntdll.dll");
	if (!hNTDLL)
	{
		return GetLastError();
	}

	FARPROC pFunc = GetProcAddress(hNTDLL, "LdrLoadDll");
	if (!pFunc)
	{
		return GetLastError();
	}

	data.pLdrLoadDll = ReCa<f_LdrLoadDll>(pFunc);

	BYTE * pArg = ReCa<BYTE*>(VirtualAllocEx(hProc, nullptr, sizeof(LDR_LOAD_DLL_DATA) + 0x200, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	if (!pArg)
	{
		return GetLastError();
	}

	if (!WriteProcessMemory(hProc, pArg, &data, sizeof(LDR_LOAD_DLL_DATA), nullptr))
	{
		VirtualFreeEx(hProc, pArg, 0, MEM_RELEASE);
		return GetLastError();
	}

	if (!WriteProcessMemory(hProc, pArg + sizeof(LDR_LOAD_DLL_DATA), LdrLoadDllShell, 0x100, nullptr))
	{
		VirtualFreeEx(hProc, pArg, 0, MEM_RELEASE);
		return GetLastError();
	}

	HANDLE hThread = StartRoutine(hProc, pArg + sizeof(LDR_LOAD_DLL_DATA), pArg);

	if (!hThread)
	{
		VirtualFreeEx(hProc, pArg, 0, MEM_RELEASE);
		return GetLastError();
	}
	else
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
	}

	VirtualFreeEx(hProc, pArg, 0, MEM_RELEASE);

	return 0;
}

bool InjectBE(const char* file, int PID) {

	if (!SetPrivilegeA("SeDebugPrivilege", true))
	{
		wprintf_s(L"Enable SeDebugPrivilege Failed, Er Code %d",(int)GetLastError());
		return FALSE;
	}

	HANDLE hProc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION, FALSE, PID);
	if (!hProc)
	{
		wprintf_s(L"OpenProcess Failed, Er Code %d", (int)GetLastError());
		return FALSE;
	}

	char buffer[MAX_PATH] = { 0 };
	GetFullPathNameA(file, MAX_PATH, buffer, NULL);
	auto ret = LdrLoadDllStub(buffer, hProc);
	CloseHandle(hProc);

	if (ret == 0)
		return TRUE;
	else
		return FALSE;

}