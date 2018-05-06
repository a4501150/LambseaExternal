#pragma once
#include <Windows.h>
#include "stdafx.h"
#include "Main.h"
#include "Helpers.h"
#include "Hooks.h"
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <WinUser.h>
#include <set>

using namespace std;

void Huorong::Initialise()
{
	Helpers::LogToFile("=============================");
	Helpers::LogToFile("Shit Is Initializing.");

	//get function pointers
	Fuckers::NtCreateFileOrig = reinterpret_cast<fNtCreateFile>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateFile"));
	Fuckers::CryptCATAdminCalcHashFromFileHandleOrig = reinterpret_cast<fCryptCATAdminCalcHashFromFileHandle>(GetProcAddress(GetModuleHandleA("Wintrust.dll"), "CryptCATAdminCalcHashFromFileHandle"));
	Fuckers::NtQueryVirtualMemoryOrig = reinterpret_cast<fNtQueryVirtualMemory>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryVirtualMemory"));
	Fuckers::NtReadVirtualMemoryOrig = reinterpret_cast<fNtReadVirtualMemory>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory"));
	Fuckers::NtQuerySystemInformationOrig = reinterpret_cast<fNtQuerySystemInformation>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation"));
	//Fuckers::K32GetDeviceDriverFileNameOrig = reinterpret_cast<fGetDeviceDriverFileName>(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "K32GetDeviceDriverFileName"));
	//Fuckers::K32EnumDeviceDriversOrig = reinterpret_cast<fK32EnumDeviceDrivers>(GetProcAddress(GetModuleHandleA("Kernel32.dll"), "K32EnumDeviceDrivers"));
	//Fuckers::CertEnumCertificatesInStoreOrig = reinterpret_cast<fCertEnumCertificatesInStore>(GetProcAddress(GetModuleHandleA("Crypt32.dll"), "CertEnumCertificatesInStore"));
	Fuckers::EnumWindowsOrig = reinterpret_cast<fEnumWindows>(GetProcAddress(GetModuleHandleA("User32.dll"), "EnumWindows"));

	//LETSHOOK
	Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtCreateFileOrig), Fuckers::NtCreateFileHook);
	Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::CryptCATAdminCalcHashFromFileHandleOrig), Fuckers::CryptCATAdminCalcHashFromFileHandleHook);
	Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtReadVirtualMemoryOrig), Fuckers::NtReadVirtualMemoryHook);
	Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtQueryVirtualMemoryOrig), Fuckers::NtQueryVirtualMemoryHook);
	Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtQuerySystemInformationOrig), Fuckers::NtQuerySystemInformationHook);
	//Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::K32GetDeviceDriverFileNameOrig), Fuckers::K32GetDeviceDriverFileNameHook);
	//Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::K32EnumDeviceDriversOrig), Fuckers::K32EnumDeviceDriversHook);
	//Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::CertEnumCertificatesInStoreOrig), Fuckers::CertEnumCertificatesInStoreHook);
	Helpers::HookFunction(reinterpret_cast<PVOID*>(&Fuckers::EnumWindowsOrig), Fuckers::EnumWindowsHook);



	//try hook again end
	Helpers::LogToFile("Original Address:");
	Helpers::LogAddressToFile("NtCreateFileOrig ", (DWORD_PTR)&Fuckers::NtCreateFileOrig);
	Helpers::LogAddressToFile("CryptCATAdminCalcHashFromFileHandleOrig ", (DWORD_PTR)&Fuckers::CryptCATAdminCalcHashFromFileHandleOrig);
	Helpers::LogAddressToFile("NtQueryVirtualMemoryOrig ", (DWORD_PTR)&Fuckers::NtQueryVirtualMemoryOrig);
	Helpers::LogAddressToFile("NtReadVirtualMemoryOrig ", (DWORD_PTR)&Fuckers::NtReadVirtualMemoryOrig);
	Helpers::LogAddressToFile("NtQuerySystemInformationOrig ", (DWORD_PTR)&Fuckers::NtQuerySystemInformationOrig);
	Helpers::LogAddressToFile("EnumWindowsOrig ", (DWORD_PTR)&Fuckers::EnumWindowsOrig);

	Helpers::LogToFile("Hook Address:");
	Helpers::LogAddressToFile("NtCreateFileHook ", (DWORD_PTR)Fuckers::NtCreateFileHook);
	Helpers::LogAddressToFile("CryptCATAdminCalcHashFromFileHandleHook ", (DWORD_PTR)Fuckers::CryptCATAdminCalcHashFromFileHandleHook);
	Helpers::LogAddressToFile("NtQueryVirtualMemoryHook ", (DWORD_PTR)Fuckers::NtQueryVirtualMemoryHook);
	Helpers::LogAddressToFile("NtReadVirtualMemoryHook ", (DWORD_PTR)Fuckers::NtReadVirtualMemoryHook);
	Helpers::LogAddressToFile("NtQuerySystemInformationHook ", (DWORD_PTR)&Fuckers::NtQuerySystemInformationHook);
	Helpers::LogAddressToFile("EnumWindowsHook ", (DWORD_PTR)&Fuckers::EnumWindowsHook);

	Helpers::LogToFile("Shit Is Live.");
	Helpers::LogToFile("=============================");


}

void Huorong::Release()
{
	
	//UNHOOK
	Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtCreateFileOrig), Fuckers::NtCreateFileHook);
	Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::CryptCATAdminCalcHashFromFileHandleOrig), Fuckers::CryptCATAdminCalcHashFromFileHandleHook);
	Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtQueryVirtualMemoryOrig), Fuckers::NtQueryVirtualMemoryHook);
	Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtReadVirtualMemoryOrig), Fuckers::NtReadVirtualMemoryHook);
	Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::NtQuerySystemInformationOrig), Fuckers::NtQuerySystemInformationHook);
	//Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::K32GetDeviceDriverFileNameOrig), Fuckers::K32GetDeviceDriverFileNameHook);
	//Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::K32EnumDeviceDriversOrig), Fuckers::K32EnumDeviceDriversHook);
	Helpers::UnhookFunction(reinterpret_cast<PVOID*>(&Fuckers::EnumWindowsOrig), Fuckers::EnumWindowsHook);
	
	Helpers::LogToFile("=============================");
	Helpers::LogToFile("Shit Release Done");
	Helpers::LogToFile("=============================");

}


