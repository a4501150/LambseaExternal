#pragma once
#include "stdafx.h"
#include "Hooks.h"
#include "Helpers.h"
#include "Main.h"
#include <set>
#include <TlHelp32.h>
#include <vector>
#include <Psapi.h>
#include <chrono>
#include <ctime>

#pragma comment(lib, "crypt32.lib")

using namespace std;

fNtCreateFile Fuckers::NtCreateFileOrig = NULL;
fCryptCATAdminCalcHashFromFileHandle Fuckers::CryptCATAdminCalcHashFromFileHandleOrig = NULL;

//fuck be's meomory scan
fNtQueryVirtualMemory Fuckers::NtQueryVirtualMemoryOrig = NULL;
fNtReadVirtualMemory Fuckers::NtReadVirtualMemoryOrig = NULL;
fNtQuerySystemInformation Fuckers::NtQuerySystemInformationOrig = NULL;

fK32EnumDeviceDrivers Fuckers::K32EnumDeviceDriversOrig = NULL;
fCertEnumCertificatesInStore Fuckers::CertEnumCertificatesInStoreOrig = NULL;

fEnumWindows Fuckers::EnumWindowsOrig = NULL;

std::wstring StringToWString(const std::string& s)
{
	std::wstring temp(s.length(), L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

std::string WStringToString(const std::wstring& s)
{
	std::string temp(s.length(), ' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}

uint32_t GetProcIDByName(const wchar_t* proc)
{
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

bool isInProtectProcIDvector(DWORD pid) {

	std::vector<uint32_t> protectedProcId;

	auto mmcid = GetProcIDByName(L"mmc.exe");
	auto notepadid = GetProcIDByName(L"resmon.exe");
	auto calcid = GetProcIDByName(L"wusa.exe");

	auto OptionalFeaturesid = GetProcIDByName(L"OptionalFeatures.exe");
	auto odbcad32id = GetProcIDByName(L"odbcad32.exe");

	auto loaderIDA = GetProcIDByName(L"lambsea.exe");
	auto loaderIDB = GetProcIDByName(L"kkloader.exe");
	auto loaderIDC = GetProcIDByName(L"ljy.exe");

	protectedProcId.push_back(mmcid);
	protectedProcId.push_back(notepadid);
	protectedProcId.push_back(calcid);
	protectedProcId.push_back(OptionalFeaturesid);
	protectedProcId.push_back(odbcad32id);
	protectedProcId.push_back(loaderIDA);
	protectedProcId.push_back(loaderIDB);
	protectedProcId.push_back(loaderIDC);


	if (std::find(protectedProcId.begin(), protectedProcId.end(), (uint32_t)pid) != protectedProcId.end() && pid!=0) {
		return TRUE;
	}
	else {
		return FALSE;
	}



}

NTSTATUS NTAPI Fuckers::NtCreateFileHook(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, void* IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength)
{

	if (ObjectAttributes &&
		ObjectAttributes->ObjectName &&
		ObjectAttributes->ObjectName->Buffer)
	{
		auto wstr = std::wstring(ObjectAttributes->ObjectName->Buffer);
		if (wstr.find(L"lxw") != std::wstring::npos || wstr.find(L"ambsea") != std::wstring::npos || wstr.find(L"cpuz141.sys") != std::wstring::npos || wstr.find(L"TASAgent.dll") != std::wstring::npos || wstr.find(L"Tenprotect.dll") != std::wstring::npos || wstr.find(L"kk") != std::wstring::npos)
		{
			
			Helpers::LogToFileEx(" ");
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx("Shit Perform an Open File");
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx("Following File Has Been Openned / Redirected:");
			Helpers::LogToFileEx(WStringToString(wstr).c_str());

			_RtlInitUnicodeString RtlInitUnicodeString = (_RtlInitUnicodeString)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlInitUnicodeString");

			UNICODE_STRING fakePath;
			RtlInitUnicodeString(&fakePath, L"\\??\\C:\\Windows\\System32\\kernel32.dll");

			auto backupPath = ObjectAttributes->ObjectName;
			ObjectAttributes->ObjectName = &fakePath;
			Helpers::LogToFileEx("Redirected Done.");
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx(" ");
		}
	}
	return NtCreateFileOrig(FileHandle, DesiredAccess, ObjectAttributes, (PIO_STATUS_BLOCK)IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
};

BOOL WINAPI Fuckers::CryptCATAdminCalcHashFromFileHandleHook(HANDLE hFile, DWORD  *pcbHash, BYTE *pbHash, DWORD dwFlags) {

	TCHAR Path[MAX_PATH];
	HANDLE BackupHandle;

	auto result = GetFinalPathNameByHandle(hFile, Path, MAX_PATH, VOLUME_NAME_NT);
	
	if (result < MAX_PATH) {

		auto wstr = std::wstring(Path);

		if (wstr.find(L"lxw") != std::wstring::npos || wstr.find(L"ambsea") != std::wstring::npos || wstr.find(L"cpuz141.sys") != std::wstring::npos || wstr.find(L"TASAgent.dll") != std::wstring::npos || wstr.find(L"Tenprotect.dll") != std::wstring::npos || wstr.find(L"kk") != std::wstring::npos)
		{
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx("Shit Perform a Hash Check");
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx("Fllowing File has been Calculated:");
			Helpers::LogToFileEx(WStringToString(wstr).c_str());
			Helpers::LogToFileEx("Replaced.");
			BackupHandle = hFile;
			hFile = CreateFile((L"\\??\\C:\\Windows\\System32\\kernel32.dll"),0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				Helpers::LogToFileEx("CreateFile In Hash Hook Failed.");
				Helpers::LogValueToFile("CreateFile In Hash Hook Failed.", GetLastError());
			}
			CryptCATAdminCalcHashFromFileHandleOrig(hFile, pcbHash, pbHash, dwFlags);
			hFile = BackupHandle;
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx(" ");
			return TRUE;
		}

	}
	else
	{
		Helpers::LogToFileEx("Obtain File Failed.");
		Helpers::LogAddressToFile("required file path is", result);
		Helpers::LogAddressToFile("Last Error", GetLastError());
	}


	return CryptCATAdminCalcHashFromFileHandleOrig( hFile,pcbHash,pbHash,dwFlags);
}

NTSTATUS Fuckers::NtReadVirtualMemoryHook(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG BufferLength, PULONG ReturnLength OPTIONAL)
{

	DWORD PID = GetProcessId(ProcessHandle);

	if (PID == GetCurrentProcessId()|| ProcessHandle == GetCurrentProcess())
	{
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Shit Perform a NtReadVirtualMemoryHook");
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Redirected Self Scan.");
		*ReturnLength = 0;
		SetLastError(299);
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		Helpers::LogToFileEx(std::ctime(&end_time));
		return STATUS_ACCESS_VIOLATION;
	}

	if (isInProtectProcIDvector(PID)) 
	{
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Shit Perform a NtReadVirtualMemory");
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Redirected Cheat Scan.");
		*ReturnLength = 0;
		SetLastError(299);
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		Helpers::LogToFileEx(std::ctime(&end_time));
		return STATUS_ACCESS_VIOLATION;
	}

	return NtReadVirtualMemoryOrig(ProcessHandle, BaseAddress, Buffer,BufferLength,ReturnLength);
}

NTSTATUS Fuckers::NtQueryVirtualMemoryHook(HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID Buffer, ULONG Length, PULONG ResultLength OPTIONAL)
{

	DWORD PID = GetProcessId(ProcessHandle);

	if (PID == GetCurrentProcessId() || ProcessHandle == GetCurrentProcess())
	{
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Shit Perform a NtQueryVirtualMemory");
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Redirected Self Scan.");
		SetLastError(299);
		*ResultLength = 0;
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		Helpers::LogToFileEx(std::ctime(&end_time));
		return STATUS_ACCESS_VIOLATION;
	}

	if (isInProtectProcIDvector(PID))
	{
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Shit Perform a NtQueryVirtualMemory");
		Helpers::LogToFileEx("=============================");
		Helpers::LogToFileEx("Redirected Cheat Scan.");
		SetLastError(299);
		*ResultLength = 0;
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		Helpers::LogToFileEx(std::ctime(&end_time));
		return STATUS_ACCESS_VIOLATION;
	}

	return NtQueryVirtualMemoryOrig(ProcessHandle, BaseAddress, MemoryInformationClass, Buffer, Length, ResultLength);
}

NTSTATUS Fuckers::NtQuerySystemInformationHook(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength)
{
	if (SystemInformationClass == 16)
	{


	}

	if (SystemInformationClass == 5) 
	{


	}

	auto ret = NtQuerySystemInformationOrig(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

	if (ret == 0xC0000004)
		return ret;

	else
	{
		ZeroMemory(SystemInformation, SystemInformationLength);
		return STATUS_ACCESS_VIOLATION;
	}

	return NtQuerySystemInformationOrig(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
}

BOOL Fuckers::K32EnumDeviceDriversHook(LPVOID * lpImageBase, DWORD cb, LPDWORD lpcbNeeded)
{
	Helpers::LogToFileEx("=============================");
	Helpers::LogToFileEx("Shit Perform a K32EnumDeviceDrivers");
	Helpers::LogToFileEx("=============================");
	return K32EnumDeviceDriversOrig(lpImageBase, cb, lpcbNeeded);
}

PCCERT_CONTEXT Fuckers::CertEnumCertificatesInStoreHook(HCERTSTORE hCertStore, PCCERT_CONTEXT pPrevCertContext)
{
	Helpers::LogToFileEx("=============================");
	Helpers::LogToFileEx("Shit Perform a CertEnumCertificatesInStore");
	Helpers::LogToFileEx("=============================");

	auto pCertContext = CertEnumCertificatesInStoreOrig(hCertStore, pPrevCertContext);

	wchar_t  pszNameString[256];

	DWORD ret = FALSE;

	try
	{
		ret = CertGetNameStringW
		(
			pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			pszNameString,
			128
		);
		wstring t = pszNameString;
		Helpers::LogToFileEx(WStringToString(t).c_str());

	}
	catch (...)
	{
		Helpers::LogValueToFile("Error", GetLastError());
	}

	if (ret)
	{
		wstring name = pszNameString;
		if (name.find(L"lxw") != wstring::npos)
		{
			Helpers::LogToFileEx("=============================");
			Helpers::LogToFileEx("Redirected Cert Scan");
			Helpers::LogToFileEx("=============================");
			pCertContext = CertEnumCertificatesInStoreOrig(hCertStore, pPrevCertContext);
		}
	}

	return pCertContext;
}

BOOL Fuckers::EnumWindowsHook(WNDENUMPROC lpEnumFunc, LPARAM lParam)
{

	Helpers::LogToFileEx("=============================");
	Helpers::LogToFileEx("EnumrateWindows");
	Helpers::LogToFileEx("=============================");

	return EnumWindowsOrig(lpEnumFunc, lParam);
}




