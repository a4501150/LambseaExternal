#pragma once
// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "LambseaExternal.h"

//编译为Dll, 入口在这里
BOOL APIENTRY DllMain
(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(hModule); // PERFOMANCE?
			CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialize), NULL, NULL, NULL);
			break;

		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

//编译为Exe, 入口在这里
BOOL WINAPI main() 
{
	CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialize), NULL, NULL, NULL);

	while (true) {
		Sleep(1000 * 60);
	}

	return 0;
}