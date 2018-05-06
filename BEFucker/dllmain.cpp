#pragma once
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
		DisableThreadLibraryCalls(hModule); // PERFOMANCE?
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Huorong::Initialise), NULL, NULL, NULL);
		break;

	case DLL_PROCESS_DETACH:
		Huorong::Release();
		break;
	}
	return TRUE;
}

