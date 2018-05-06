#pragma once
#include "Exports.h"
#include "LambseaDriver.h"
#include <vector>
#include <iostream>
#include <time.h>
#include <TlHelp32.h>

struct signutureExes {

	std::wstring exename;
	std::wstring fullPath;

	signutureExes(std::wstring ename, std::wstring path) {
		exename = ename;
		fullPath = path;
	}
};

std::uint32_t GetProcIDByName(const wchar_t* proc)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	auto pe = PROCESSENTRY32W{ sizeof(PROCESSENTRY32W) };
	uint32_t id = 0;

	if (Process32First(snapshot, &pe)) {
		do {
			if (!_wcsicmp(proc, pe.szExeFile)) {
				id = pe.th32ProcessID;
			}
		} while (Process32Next(snapshot, &pe));
	}
	CloseHandle(snapshot);
	return id;
}

BOOL LoadLibraryInject(const wchar_t* procname, const char* dllpath) {

	HANDLE processHandle;
	HANDLE threadHandle;
	HMODULE dllHandle;
	DWORD processID;
	FARPROC loadLibraryAddress;
	LPVOID baseAddress;

	const char* args = dllpath;

	processID = GetProcIDByName(procname);

	processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
		PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, processID);

	if (processHandle == NULL)
	{
		printf("Error unable to open process. Error code: %d", GetLastError());
		return FALSE;
	}

	dllHandle = GetModuleHandle(L"Kernel32");

	if (dllHandle == NULL)
	{
		printf("Error unable to allocate kernel32 handle..Error code: %d. Press any key to exit...", GetLastError());
		return FALSE;
	}

	loadLibraryAddress = GetProcAddress(dllHandle, "LoadLibraryA");

	if (loadLibraryAddress == NULL)
	{
		printf("Cannot get LoadLibraryA() address. Error code: %d. Press any key to exit", GetLastError());
		std::cin.get();

		return FALSE;
	}

	baseAddress = VirtualAllocEx(
		processHandle,
		NULL,
		256,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE);

	if (baseAddress == NULL)
	{
		printf("Error unable to alocate memmory in remote process. Error code: %d. Press any key to exit", GetLastError());
		return FALSE;
	}

	BOOL isSucceeded = WriteProcessMemory(
		processHandle,
		baseAddress,
		args,
		strlen(args) + 1,
		NULL);

	if (isSucceeded == 0)
	{
		printf("Error unable to write memory . Error code: %d Press any key to exit...\n", GetLastError());
		return FALSE;
	}

	threadHandle = CreateRemoteThread(
		processHandle,
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)loadLibraryAddress,
		baseAddress,
		NULL,
		0);

	if (threadHandle == NULL)
	{
		printf("Remote thread Create Failed\n");
	}
	else
	{
		NtClose(threadHandle);
	}

	printf("Successful Inject");
	return TRUE;

}

void DriverLdr(BOOL ForceLoad) {

	AdjustRights(L"SeLoadDriverPrivilege", true);

	SafeUnloadDriver(L"cpuz141");
	SafeUnloadDriver(L"Lambsea");
	SafeDeleteFile(L"Lambsea.sys", false);
	DropFile(Lambsea, L"Lambsea.sys", sizeof(Lambsea));

	BOOL ret;
	if (ForceLoad)
		ret = LoadUnsignedDriver(L"Lambsea");
	else
		ret = LoadDriver(L"Lambsea");

	if (ret)
		printf("Load Done\n");
	else
	{
		MessageBox(
			NULL,
			L"可能是因为Secureboot没有禁用 请尝试强力启动选项(此选项可能会蓝屏) 强烈建议关闭Secureboot",
			L"Error",
			MB_ICONEXCLAMATION | MB_YESNO
		);
		SafeDeleteFile(L"Lambsea.sys", false);
		DeleteDriverRegKey(L"Lambsea");
		ExitProcess(0);
	}
		
	SafeDeleteFile(L"cpuz141.sys", true);
	DeleteDriverRegKey(L"cpuz141");

}

int main(int argc, char* argv[]) {
	
	if (!argv[1]) {
		printf("you are banned.\n");
		Sleep(2000);
		return 0;
	}
	std::string argv1 = argv[1];

	if (argv1 != "lijinyang") {
		printf("you are banned.\n");
		Sleep(2000);
		return 0;
	}

	argv1.clear();

	if(!argv[2])
		DriverLdr(false);
	else
		DriverLdr(true);

	auto dllPath = "Tenprotect.dll";
	char buffer[MAX_PATH] = { 0 };
	GetFullPathNameA(dllPath, MAX_PATH, buffer, NULL);

	std::vector<signutureExes> signutureEXEs;

	//signutureEXEs.push_back(signutureExes(L"chrome.exe", L"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe"));
	signutureEXEs.push_back(signutureExes(L"resmon.exe", L"C:\\Windows\\System32\\resmon.exe"));
	signutureEXEs.push_back(signutureExes(L"mmc.exe", L"C:\\Windows\\System32\\mmc.exe"));
	signutureEXEs.push_back(signutureExes(L"OptionalFeatures.exe", L"C:\\Windows\\System32\\OptionalFeatures.exe"));
	signutureEXEs.push_back(signutureExes(L"odbcad32.exe", L"C:\\Windows\\System32\\odbcad32.exe"));

	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	//randomly choose a signutre exe to load
	int min = 0, max = signutureEXEs.size() - 1;

	srand(time(NULL));
	auto picked = min + (rand() % static_cast<int>(max - min + 1));

	// start the program up
	auto createResult = CreateProcess(signutureEXEs[picked].fullPath.c_str(),   // the path
		NULL,           // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		DETACHED_PROCESS | CREATE_SUSPENDED,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);

	if (!createResult) {
		printf("CreateProcess Failed, Error： %d \n", GetLastError());
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return 0;
	}

	//LoadLibraryInject(L"GameOverlayUI.exe", buffer);

	LoadLibraryInject(signutureEXEs[picked].exename.c_str(), buffer);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
