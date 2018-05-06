// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "Main.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Mainroutine), NULL, NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//编译为Exe, 入口在这里
BOOL WINAPI main()
{
	CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Mainroutine), NULL, NULL, NULL);

	while (true) {
		Sleep(1000 * 60);
	}

	return 0;
}
