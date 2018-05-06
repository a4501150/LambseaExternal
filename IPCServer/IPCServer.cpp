#pragma once
#include "IPCServer.h"
#include "HandleGetter.h"
#define Gamename L"TslGame.exe"

ServerGateway::ServerGateway()
{
	this->PipeName = PIPENAME;
	//printf("[ok] Wait Client\n");

	while (TRUE) {

		this->hNamedPipe = CreateNamedPipe(PipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAXPIPEFILESIZE, MAXPIPEFILESIZE, 0, NULL);
		// Wait for the client to connect; if it succeeds, the function returns a nonzero value. If the function returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
		auto m_clientConnected = ConnectNamedPipe(hNamedPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (hNamedPipe == INVALID_HANDLE_VALUE) {
			//printf("ERROR ]> CreateNamedPipe failed. GetLastError: %d\n" ,GetLastError());
			return;
		}

		if (m_clientConnected) {
			//printf("[ok] Client Connected\n");
			Init();
		}
		else {
			CloseHandle(hNamedPipe); // The client could not connect, so close the pipe. 
		}

	}

}

ServerGateway::~ServerGateway()
{
}

void ServerGateway::Init()
{
	HANDLE hHeap = GetProcessHeap();
	void* rquestBuffer = HeapAlloc(hHeap, 0, MAXPIPEFILESIZE);
	BOOL bSuccess;
	DWORD cbBytesRead = 0;

	hGame = GetHandleToProcess(Gamename);

	while (TRUE) {

		bSuccess = ReadFile(hNamedPipe, rquestBuffer, MAXPIPEFILESIZE, &cbBytesRead, NULL);

		if (!bSuccess || cbBytesRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				//printf("INFO  ]> Broken pipe (Client disconnected). GetLastError: %d\n", GetLastError());
			}
			else {
				//printf("ERROR ]> ReadFile failed. GetLastError: %d\n", GetLastError());
			}
			break;
		}

		IPCPacket incomingPacket;
		memcpy(&incomingPacket, rquestBuffer, sizeof(IPCPacket));

		switch (incomingPacket.Order) {

		case IPCPing:
			//printf("ping\n");

			break;
		case IPCGetBaseOrder:
			//printf("IPCGetBaseOrder\n");
			AnswerGetBaseaddress();
			break;
		case IPCReadOrder:
			//printf("IPCReadOrder\n");
			//printf("address 0x%I64X\n", incomingPacket.Address);
			AnswerReadProcessMemory(incomingPacket);
			break;
		case IPCWriteOrder:
			//printf("IPCWriteOrder\n");
			//printf("address 0x%I64X\n", incomingPacket.Address);
			AnswerWriteProcessMemory(incomingPacket);
			break;

		}

	}

	// Flush the pipe to allow the client to read the pipe's contents before disconnecting.
	// Then disconnect the pipe, and close the handle to this pipe instance. 
	FlushFileBuffers(hNamedPipe);
	DisconnectNamedPipe(hNamedPipe);
	CloseHandle(hNamedPipe);
	HeapFree(hHeap, 0, rquestBuffer);
	//printf("[ok] Gateway closing.\n");
}

BOOL ServerGateway::AnswerGetBaseaddress()
{
	DWORD_PTR lpBase = 0x0;
	int triedTime = 0;

	while (TRUE)
	{
		HMODULE hMods[512];
		DWORD cb;
		if (EnumProcessModulesEx(hGame, hMods, sizeof(hMods), &cb, LIST_MODULES_ALL))
		{
			wchar_t szModName[MAX_PATH] = { NULL };
			for (int i = 0; i < cb / sizeof(HMODULE); i++)
			{
				if (GetModuleBaseName(hGame, hMods[i], szModName, MAX_PATH))
				{
					if (!std::wcscmp(szModName, Gamename))
					{
						lpBase = (DWORD_PTR)hMods[i];
						break;
					}
				}
				else
				{
					break;
				}
				ZeroMemory(szModName, MAX_PATH);
			}
		}
		else
		{
			//printf("EnumProcessModulesEx failed\n");
			triedTime++;
			Sleep(500);
			if (triedTime >= 4)
			{
				lpBase = 0x0;
				break;
			}
		}
		if (lpBase != 0)
			break;
	}

	IPCPacket out;
	out.Address = lpBase;
	out.size = sizeof(lpBase);
	//printf("Readed 0x%I64X\n", out.Address);
	DWORD bytesWrittenInPipe = 0;
	auto fSuccess = WriteFile(hNamedPipe, &out, sizeof(out), &bytesWrittenInPipe, NULL);
	return fSuccess;


}

BOOL ServerGateway::AnswerReadProcessMemory(IPCPacket incomingPacket)
{
	DWORD_PTR address = incomingPacket.Address;
	DWORD_PTR size = incomingPacket.size;

	__try
	{
		auto ret = ReadProcessMemory(hGame, (LPCVOID)address, &incomingPacket.data, size, NULL);
		if (!ret)
			incomingPacket.size = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		incomingPacket.size = 0;
	}

	DWORD bytesWrittenInPipe = 0;
	auto fSuccess = WriteFile(hNamedPipe, &incomingPacket, sizeof(incomingPacket), &bytesWrittenInPipe, NULL);
	return fSuccess;

}

BOOL ServerGateway::AnswerWriteProcessMemory(IPCPacket incomingPacket)
{
	DWORD_PTR address = incomingPacket.Address;
	DWORD_PTR size = incomingPacket.size;

	__try
	{
		auto ret = WriteProcessMemory(hGame, (LPVOID)address, &incomingPacket.data, size, NULL);
		if (!ret)
			incomingPacket.size = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		incomingPacket.size = 0;
	}

	DWORD bytesWrittenInPipe = 0;
	auto fSuccess = WriteFile(hNamedPipe, &incomingPacket, sizeof(incomingPacket), &bytesWrittenInPipe, NULL);
	return fSuccess;
}
