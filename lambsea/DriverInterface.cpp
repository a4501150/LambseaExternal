#include "stdafx.h"
#include "DriverInterface.h"

#pragma region
namespace resrvlib {

	reserve_init ResInit = NULL;
	reserve_module ResModule = NULL;
	reserve_read ResRead = NULL;
	reserve_write ResWrite = NULL;

	LPCSTR szModuleName = "";
	LPCWSTR wszProcName = L"";
	unsigned int ProcID = NULL;

}

void resrvlib::Init(LPCSTR modulename, LPCWSTR processname)
{
	szModuleName = modulename;
	wszProcName = processname;
	//ProcID = GetProcessID();
}

BOOL resrvlib::LoadDriver()
{
	HMODULE resdll = LoadLibrary(L"resrvlib64.dll");
	if (!resdll)
		wprintf_s( L"LoadLibrary error GetLastError: %d", GetLastError());

	ResInit = reinterpret_cast<reserve_init>(
		GetProcAddress(resdll, "reserve_init"));

	ResModule = reinterpret_cast<reserve_module>(
		GetProcAddress(resdll, "reserve_module"));

	ResRead = reinterpret_cast<reserve_read>(
		GetProcAddress(resdll, "reserve_read"));

	ResWrite = reinterpret_cast<reserve_write>(
		GetProcAddress(resdll, "reserve_write"));

	if (ResInit())
		return TRUE;
	else
	{
		wprintf_s(L"初始化驱动模块失败..也许是驱动已经被加载 >> Error Code %d\n", GetLastError());
		return FALSE;
	}
		

}

BOOL resrvlib::UnLoadDriver()
{
	return 0;
}

DWORD_PTR resrvlib::DriverGetModulebase( OPTIONAL ULONG size)
{

	if (ProcID == NULL)
	{
		wprintf_s(L"无法获得进程ID\n");
		return FALSE;
	}
	else
	{
		wprintf_s(L"游戏ID...%d\n",(int)ProcID);
	}

	DWORD_PTR baseaddr;

	auto ret = ResModule(ProcID, szModuleName, &baseaddr, &size);

	if (ret)
		return baseaddr;
	else
		return FALSE;
}

BOOL resrvlib::DriverReadMemory(ULONGLONG lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	//if (ProcID == NULL)
	//{
	//	wprintf_s(L"无法获得进程ID\n");
	//	return FALSE;
	//}

	auto ret = ResRead(ProcID, lpBaseAddress, lpBuffer, nSize);
	return ret;

}

BOOL resrvlib::DriverWriteMemory(ULONGLONG lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	if (ProcID == NULL)
	{
		wprintf_s(L"无法获得进程ID\n");
		return FALSE;
	}

	auto ret = ResWrite(ProcID, lpBaseAddress, lpBuffer, nSize);
	return ret;
}

uint32_t GetProcessID()
{
	const wchar_t * proc = resrvlib::wszProcName;

	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	auto pe = PROCESSENTRY32W{ sizeof(PROCESSENTRY32W) };

	if (Process32First(snapshot, &pe)) {
		do {
			if (!_wcsicmp(proc, pe.szExeFile)) {
				CloseHandle(snapshot);
				return pe.th32ProcessID;
			}
		} while (Process32Next(snapshot, &pe));
	}
	CloseHandle(snapshot);
	return 0;
}


#pragma endregion



HANDLE LambseaDriver::hDriver = INVALID_HANDLE_VALUE;
ULONG LambseaDriver::ProcID = 0;
void LambseaDriver::Init(LPCSTR modulename, LPCWSTR processname)
{

	if (!hDriver)
	{
		printf("cannot connect to driver");
		return;
	}

	DWORD Bytes;
	DeviceIoControl(hDriver, IO_Init_REQUEST, &ProcID, sizeof(ProcID), &ProcID, sizeof(ProcID), &Bytes, NULL);

	ULONG CID = GetCurrentProcessId();
	DeviceIoControl(hDriver, IO_PROTECT_REQUEST, &CID, sizeof(CID), &CID, sizeof(CID), &Bytes, NULL);

}

BOOL LambseaDriver::LoadDriver()
{
	hDriver = CreateFile(RegPath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_TEMPORARY, 0);
	if (hDriver != INVALID_HANDLE_VALUE)
	{
		DWORD Bytes;
		ULONG CID = GetCurrentProcessId();
		DeviceIoControl(hDriver, IO_PROTECT_REQUEST, &CID, sizeof(CID), &CID, sizeof(CID), &Bytes, NULL);
		return TRUE;
	}
		
	else
		return FALSE;
}

BOOL LambseaDriver::UnLoadDriver()
{
	//DWORD Bytes;
	//ULONG CID = 0;
	//DeviceIoControl(hDriver, IO_PROTECT_REQUEST, &CID, sizeof(CID), &CID, sizeof(CID), &Bytes, NULL);

	if (CloseHandle(hDriver))
		return TRUE;
	else
		return FALSE;
}

DWORD_PTR LambseaDriver::DriverGetModulebase(OPTIONAL ULONG size)
{
	if (hDriver == INVALID_HANDLE_VALUE || !hDriver)
		return false;

	DWORD_PTR Address;
	DWORD Bytes;

	if (DeviceIoControl(hDriver, IO_GET_MODULE_REQUEST, &Address, sizeof(Address), &Address, sizeof(Address), &Bytes, NULL))
		return Address;
	else
		return false;
}



BOOL LambseaDriver::DriverWriteMemory(ULONGLONG lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize)
{
	if (hDriver == INVALID_HANDLE_VALUE || !hDriver)
		return false;

	DWORD Bytes;

	KERNEL_WRITE_REQUEST  WriteRequest;
	WriteRequest.CopiedFromAddress = (DWORD_PTR)lpBuffer;
	WriteRequest.TargetAddress = lpBaseAddress;
	WriteRequest.Size = nSize;

	if (DeviceIoControl(hDriver, IO_WRITE_REQUEST, &WriteRequest, sizeof(WriteRequest), &WriteRequest, sizeof(WriteRequest), &Bytes, NULL))
		return true;
	else
		return false;
}
